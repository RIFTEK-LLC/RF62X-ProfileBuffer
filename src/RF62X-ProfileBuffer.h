#pragma once
#include <thread>
#include <mutex>
#include <string.h>
#include <sstream>
#include <memory>
#include <vector>
#include <map>

#include "rf62Xsdk.h"

namespace SDK {
namespace UTILS {

enum class ProfileBufferOptions
{
    /// Enable(Disable) zero points in capturing profile2D
    ZERO_POINTS,
    /// Enable(Disable) socket buffering (if socket buffering is disabled,
    /// profiles can be lost, but the lag from real time is minimal)
    REALTIME,
    /// Enable(Disable) profile loss detection
    LOSS_DETECTION
};

/**
 * @class
 *
 * @brief Implement profile buffering
 */
class ProfileBuffer
{
public:
    /**
     * @brief Method to get string of current library version.
     *
     * @return String of current library version.
     */
    static std::string getVersion();

    /**
     * @brief Class constructor.
     *
     * @param capacity Buffer capacity
     */
    ProfileBuffer(int capacity = 10000);

    /**
     * @brief Class destructor.
     */
    ~ProfileBuffer();

    /**
     * @brief Method for set the scanner for profile receiving
     *
     * @param scanner Scanner object for profile receiving
     *
     * @return true on success, else - false
     */
    bool setScanner(std::shared_ptr<SCANNERS::RF62X::rf627smart> scanner);

    /**
     * @brief Method for set the profile handling function
     *
     * @param func Profile handling function
     */
    void setProfileHandler(
            std::function<bool(std::shared_ptr<SCANNERS::RF62X::profile2D>)>func);

    /**
     * @brief Method for set the error handling function
     *
     * @param func Error handling function
     */
    void setErrorHandler(std::function<bool(std::string)> func);

    /**
     * @brief Method for set option value
     *
     * @param option Option ID
     * @param value Option value
     *
     * @return true on success, else - false
     */
    bool setOption(ProfileBufferOptions id, bool value);

    /**
     * @brief Method to enabling profile capturing
     *
     * @return true on success, else - false
     */
    bool startCapturing();

    /**
     * @brief Method to disabling profile capturing
     *
     * @return true on success, else - false
     */
    bool stopCapturing();

    /**
     * @brief Method for get option value
     *
     * @param option Option ID
     * @param value Option value
     *
     * @return true on success, else - false
     */
    bool getOption(ProfileBufferOptions id, bool& value);

    /**
     * @brief Method for getting the number of profiles in the buffer
     *
     * @return the number of profiles in the buffer
     */
    int getSize();

    /**
     * @brief Method to returns pointer to the last profile in the buffer
     *
     * @return the last profile in the buffer
     */
    std::shared_ptr<SCANNERS::RF62X::profile2D> getBack();

    /**
     * @brief Method to returns pointer to the first profile in the buffer
     *
     * @return the first profile in the buffer
     */
    std::shared_ptr<SCANNERS::RF62X::profile2D> getFront();

    /**
     * @brief Method to returns vector of all profiles in the buffer
     *
     * @return vector of profiles
     */
    std::vector<std::shared_ptr<SCANNERS::RF62X::profile2D>> getAll();

    /**
     * @brief Buffer flush method
     *
     * @return true on success, else - false
     */
    bool clearBuffer();

    /**
     * @brief Get information about the last error
     *
     * @return error info
     */
    std::string getErrorInfo();

private:
    /// profile capture method
    void _capturing();
    /// error setting method
    void _setErrorMsg(std::string);
    /// profile check loss profiles
    void _checkLostProfiles(std::shared_ptr<SCANNERS::RF62X::profile2D>);

private:
    /// Buffer capacity
    int m_bufferCapacity{10000};
    /// Buffer size
    int m_bufferSize{0};
    /// Read position in buffer
    int m_readPosition{0};
    /// Write position in buffer
    int m_writePosition{0};
    /// Buffer mutex
    std::mutex m_bufferMutex;
    /// Buffer of profiles
    std::shared_ptr<SCANNERS::RF62X::profile2D>* m_buffer;
    /// Scanner for buffer capturing
    std::shared_ptr<SCANNERS::RF62X::rf627smart> m_scanner{nullptr};

    /// Zero points flag
    bool m_zeroPoints{true};
    /// Real-time flag
    bool m_realtime{false};
    /// Loss detection flag
    bool m_lossDetection{false};
    /// Loss detection profile index
    long m_lastMeasureCount{-1};
    /// Last error info
    std::string m_lastErrorInfo{""};

    /// thread status
    bool m_run{false};
    /// thread stop flag
    bool m_stopFlag{false};
    /// thread for capturing
    std::thread m_thread;

    /// callback for profile capturing
    std::function<bool(std::shared_ptr<SCANNERS::RF62X::profile2D>)> m_profileFunc{nullptr};
    /// callback for error capturing
    std::function<bool(std::string)> m_errorFunc{nullptr};
};

}
}


