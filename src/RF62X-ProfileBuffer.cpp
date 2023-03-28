#include "RF62X-ProfileBuffer.h"
#include "RF62X-ProfileBufferVersion.h"

using namespace SDK::UTILS;
using namespace SDK::SCANNERS::RF62X;

std::string ProfileBuffer::getVersion()
{
    return PROFILE_BUFFER_VERSION;
}

ProfileBuffer::ProfileBuffer(int capacity)
    : m_bufferCapacity(capacity)
    , m_buffer(new std::shared_ptr<SCANNERS::RF62X::profile2D>[m_bufferCapacity])
{

}

ProfileBuffer::~ProfileBuffer()
{
    std::unique_lock<std::mutex> lock(m_bufferMutex);
    if (m_run && m_thread.joinable())
    {
        m_stopFlag = true;
        m_thread.join();
        m_run = false;
        m_lastMeasureCount = -1;
    }
    delete[] m_buffer;
}

bool ProfileBuffer::startCapturing()
{
    std::unique_lock<std::mutex> lock(m_bufferMutex);
    if (!m_run)
    {
        if (true){
            m_thread = std::thread(&ProfileBuffer::_capturing, this);
            m_run = true;
            return true;
        }else
        {
            _setErrorMsg("The scanner has not been selected");
            return false;
        }
    }
    lock.unlock();

    _setErrorMsg("The capture thread is already running");
    return false;
}

bool ProfileBuffer::stopCapturing()
{
    if (m_run && m_thread.joinable())
    {
        std::unique_lock<std::mutex> lock(m_bufferMutex);
        m_stopFlag = true;
        lock.unlock();
        m_thread.join();
        m_run = false;
        m_lastMeasureCount = -1;
        return true;
    }

    _setErrorMsg("The capture thread has already been stopped");
    return false;
}

bool ProfileBuffer::getOption(ProfileBufferOptions id, bool &value)
{
    // Check options ID.
    switch (id)
    {
    // zero points flag
    case ProfileBufferOptions::ZERO_POINTS:
    {
        value = m_zeroPoints;
        return true;
    }
    // real-time flag
    case ProfileBufferOptions::REALTIME:
    {
        value = m_realtime;
        return true;
    }
    // loss detection flag
    case ProfileBufferOptions::LOSS_DETECTION:
    {
        value = m_lossDetection;
        return true;
    }
    default:
        return false;
    }
}

bool ProfileBuffer::clearBuffer()
{
    std::unique_lock<std::mutex> lock(m_bufferMutex);
    delete[] m_buffer;
    m_buffer = new std::shared_ptr<SCANNERS::RF62X::profile2D>[m_bufferCapacity];
    m_readPosition = 0;
    m_writePosition = 0;
    m_lastMeasureCount = -1;
    return true;
}

bool ProfileBuffer::setScanner(std::shared_ptr<SCANNERS::RF62X::rf627smart> scanner)
{
    std::unique_lock<std::mutex> lock(m_bufferMutex);
    m_scanner = scanner;
    return true;
}

void ProfileBuffer::setProfileHandler(std::function<bool (std::shared_ptr<SCANNERS::RF62X::profile2D>)> func)
{
    m_profileFunc = func;
}

void ProfileBuffer::setErrorHandler(std::function<bool (std::string)> func)
{
    m_errorFunc = func;
}

bool ProfileBuffer::setOption(ProfileBufferOptions id, bool value)
{
    // Check options ID.
    switch (id)
    {
    // zero points flag
    case ProfileBufferOptions::ZERO_POINTS:
    {
        m_zeroPoints = value;
        return true;
    }
    // real-time flag
    case ProfileBufferOptions::REALTIME:
    {
        m_realtime = value;
        return true;
    }
    // loss detection flag
    case ProfileBufferOptions::LOSS_DETECTION:
    {
        m_lossDetection = value;
        return true;
    }
    default:
        return false;
    }
}

int ProfileBuffer::getSize()
{
    std::unique_lock<std::mutex> lock(m_bufferMutex);
    return m_writePosition >= m_readPosition
            ? m_writePosition - m_readPosition
            : (m_bufferCapacity - m_readPosition) + m_writePosition;
}

std::shared_ptr<profile2D> ProfileBuffer::getBack()
{
    std::unique_lock<std::mutex> lock(m_bufferMutex);
    if (m_writePosition != m_readPosition)
    {
        m_writePosition--;
        if (m_writePosition < 0) m_writePosition += m_bufferCapacity;
        return m_buffer[m_writePosition];
    }
    lock.unlock();

    _setErrorMsg("The buffer is empty");
    return nullptr;
}

std::shared_ptr<profile2D> ProfileBuffer::getFront()
{
    std::unique_lock<std::mutex> lock(m_bufferMutex);
    if (m_writePosition != m_readPosition)
    {
        m_readPosition++;
        if (m_readPosition >= m_bufferCapacity) m_readPosition = 0;
        return m_buffer[m_readPosition];
    }
    lock.unlock();

    _setErrorMsg("The buffer is empty");
    return nullptr;
}

std::vector<std::shared_ptr<profile2D>> ProfileBuffer::getAll()
{
    if (m_writePosition >= m_readPosition)
    {
        std::vector<std::shared_ptr<profile2D>> result(&m_buffer[m_readPosition], &m_buffer[m_writePosition]);
        m_readPosition = m_writePosition;
        return result;
    }else
    {
        std::vector<std::shared_ptr<profile2D>> result(&m_buffer[m_readPosition], &m_buffer[m_bufferCapacity]);
        result.insert(result.end(), &m_buffer[0], &m_buffer[m_writePosition]);
        m_readPosition = m_writePosition;
        return result;
    }
}

std::string ProfileBuffer::getErrorInfo()
{
    return m_lastErrorInfo;
}

void ProfileBuffer::_capturing()
{
    while (!m_stopFlag) {
        std::shared_ptr<profile2D> profile = nullptr;
        if ((profile=m_scanner->get_profile2D(m_zeroPoints,m_realtime)))
        {
            // Check lost profiles
            if (m_lossDetection)
                _checkLostProfiles(profile);

            // process in m_profileFunc and(or) add profile to buffer
            if (!m_profileFunc || (m_profileFunc && !m_profileFunc(profile)))
            {
                std::unique_lock<std::mutex> lock(m_bufferMutex);
                if (!m_stopFlag)
                {
                    m_buffer[m_writePosition] = profile;
                    m_writePosition++;
                    m_writePosition = m_writePosition % m_bufferCapacity;
                    if (m_writePosition == m_readPosition)
                    {
                        m_readPosition++;
                        m_readPosition = m_readPosition % m_bufferCapacity;
                    }
                }
            }
        }else
        {
            _setErrorMsg("Profile not received");
        }
    }
    m_stopFlag = false;
}

void ProfileBuffer::_setErrorMsg(std::string msg)
{
    if (!m_errorFunc || ((m_errorFunc) && !m_errorFunc(msg)))
        m_lastErrorInfo = msg;
}

void ProfileBuffer::_checkLostProfiles(std::shared_ptr<SCANNERS::RF62X::profile2D> profile)
{
    long measureCount = profile->getHeader().measure_count;
    if (m_lastMeasureCount == -1)
    {
        m_lastMeasureCount = measureCount;
    }
    else
    {
        long count = 0;
        bool detected = false;

        if (measureCount > m_lastMeasureCount)
        {
            if (measureCount - m_lastMeasureCount == 1)
                return;
            else {
                count = measureCount - m_lastMeasureCount;
                detected = true;
            }
        }else if (m_lastMeasureCount > measureCount)
        {
            if (m_lastMeasureCount - measureCount == 0xFFFFFFFF)
                return;
            else
            {
                count = 0xFFFFFFFF - m_lastMeasureCount + measureCount;
                detected = true;
            }
        }

        if (detected)
            _setErrorMsg("Profile loss detected. Lost: " + std::to_string(count));
    }
}
