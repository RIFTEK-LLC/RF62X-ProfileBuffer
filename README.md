## CONTENTS
- [OVERVIEW](#overview)
- [API DESCRIPTION](#api-description)
  - [CTOR and DTOR](#ctor-and-dtor)
  - [getVersion(..)](#getversion)
  - [setScanner(..)](#setscanner)
  - [setProfileHandler(..)](#setprofilehandler)
  - [setErrorHandler(..)](#seterrorhandler)
  - [setOption(..)](#setoption)
  - [startCapturing(..)](#startcapturing)
  - [stopCapturing(..)](#stopcapturing)
  - [getOption(..)](#getoption)
  - [getSize(..)](#getsize)
  - [getBack(..)](#getback)
  - [getFront(..)](#getfront)
  - [getAll(..)](#getall)
  - [clearBuffer(..)](#clearbuffer)
  - [getErrorInfo(..)](#geterrorinfo)
- [USAGE EXAMPLE](#usage-example)
  - [DemoApp](#demoapp)
  

## OVERVIEW

This project simplifies the creation applications with profiles buffering.

## API DESCRIPTION
The **RF62X-ProfileBuffer** contains one class `ProfileBuffer` which can be used 
to simplify receiving profiles from the scanner. 

### CTOR and DTOR

```cpp
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
```

##### Description

*Class constructor and destructor*

### getVersion(..)

```cpp
/**
 * @brief Method to get string of current version of library.
 *
 * @return String of current library version.
 */
static std::string getVersion();
```

##### Description

*Static method to get string of current version of library.*

### setScanner(..)

```cpp
/**
 * @brief Method for set the scanner for profile receiving
 *
 * @param scanner Scanner object for profile receiving
 *
 * @return true on success, else - false
 */
bool setScanner(std::shared_ptr<SCANNERS::RF62X::rf627smart> scanner);
```

##### Description

*Method for set the scanner for profile receiving.*

### setProfileHandler(..)

```cpp
/**
 * @brief Method for set the profile handling function
 *
 * @param func Profile handling function
 */
void setProfileHandler(
        std::function<bool(std::shared_ptr<SCANNERS::RF62X::profile2D>)>func);
```

##### Description

*Method for set the profile handling function.*

### setErrorHandler(..)

```cpp
/**
 * @brief Method for set the error handling function
 *
 * @param func Error handling function
 */
void setErrorHandler(std::function<bool(std::string)> func);
```

##### Description

*Method for set the error handling function.*

### setOption(..)

```cpp
/**
 * @brief Method for set option value
 *
 * @param option Option ID
 * @param value Option value
 *
 * @return true on success, else - false
 */
bool setOption(ProfileBufferOptions id, bool value);
```

##### Description

*Method for set option value.*

### startCapturing(..)

```cpp
/**
 * @brief Method to enabling profile capturing
 *
 * @return true on success, else - false
 */
bool startCapturing();
```

##### Description

*Method to enabling profile capturing.*

### stopCapturing(..)

```cpp
/**
 * @brief Method to disabling profile capturing
 *
 * @return true on success, else - false
 */
bool stopCapturing();
```

##### Description

*Method to disabling profile capturing.*

### getOption(..)

```cpp
/**
 * @brief Method for get option value
 *
 * @param option Option ID
 * @param value Option value
 *
 * @return true on success, else - false
 */
bool getOption(ProfileBufferOptions id, bool& value);
```

##### Description

*Method for get option value.*

### getSize(..)

```cpp
/**
 * @brief Method for getting the number of profiles in the buffer
 *
 * @return the number of profiles in the buffer
 */
int getSize();
```

##### Description

*Method for getting the number of profiles in the buffer.*

### getBack(..)

```cpp
/**
 * @brief Method to returns pointer to the last profile in the buffer
 *
 * @return the last profile in the buffer
 */
std::shared_ptr<SCANNERS::RF62X::profile2D> getBack();
```

##### Description

*Method to returns pointer to the last profile in the buffer.*

### getFront(..)

```cpp
/**
 * @brief Method to returns pointer to the first profile in the buffer
 *
 * @return the first profile in the buffer
 */
std::shared_ptr<SCANNERS::RF62X::profile2D> getFront();
```

##### Description

*Method to returns pointer to the first profile in the buffer.*

### getAll(..)

```cpp
/**
 * @brief Method to returns vector of all profiles in the buffer
 *
 * @return vector of profiles
 */
std::vector<std::shared_ptr<SCANNERS::RF62X::profile2D>> getAll();
```

##### Description

*Method to returns vector of all profiles in the buffer.*

### clearBuffer(..)

```cpp
/**
 * @brief Buffer flush method
 *
 * @return true on success, else - false
 */
bool clearBuffer();
```

##### Description

*Buffer flush method.*

### getErrorInfo(..)

```cpp
/**
 * @brief Get information about the last error
 *
 * @return error info
 */
std::string getErrorInfo();
```

##### Description

*Get information about the last error.*

## USAGE EXAMPLE

### DemoApp
Simple DemoApp for receiving profiles:

```cpp
#include <iostream>
#include <cstring>
#include <thread>
#include <ctime>
#include <chrono>
#include <fstream>

#include "RF62X-ProfileBuffer.h"
using namespace SDK::UTILS;
using namespace SDK::SCANNERS;

/// Example of a class that consumes error messages
class ErrorHandler
{
public:
    bool print(std::string info){
        std::cout << "ERROR: " << info << std::endl;
        return true;
    }
};

/// Example of a class that consumes profiles
class ProfileHandler
{
public:
    bool process(std::shared_ptr<RF62X::profile2D> profile){
        std::cout << "Profile #" << profile->getHeader().measure_count
                  << " processed" << std::endl;

        /// if the method returns "true", it means that the profile has been
        /// processed and will not be added to the storage buffer. Otherwise,
        /// if the method returns "false", then the profile will be added.
        return false;
    }
};

// Entry point.
int main(void)
{
    std::cout
    << "======================================================" << std::endl
    << "RF62X Profile Buffer Demo App"                          << std::endl
    << "======================================================" << std::endl
    << "Library versions: "                                     << std::endl
    << "RF62X-SDK:.............."<< RF62X::sdk_version()        << std::endl
    << "RF62X-ProfileBuffer:...."<< ProfileBuffer::getVersion() << std::endl
    << "------------------------------------------------------" << std::endl
    << std::endl;

    // Initialize sdk library
    RF62X::sdk_init();

    // Create value for scanners vector's type
    std::vector<std::shared_ptr<RF62X::rf627smart>> list;
    // Search for rf627smart devices over network
    list = RF62X::rf627smart::search();

    // Print count of discovered rf627smart in network by Service Protocol
    std::cout << "Was found\t: " << list.size()<< " RF627 v2.x.x"<< std::endl;
    std::cout << "========================================="     << std::endl;

    int index = -1;
    if (list.size() > 1) {
        std::cout << "Select scanner for test: " << std::endl;
        for (size_t i = 0; i < list.size(); i++)
            std::cout << i << ". Serial: "
                      << list[i]->get_info()->serial_number() << std::endl;
        std::cin >> index;
    }
    else if (list.size() == 1) {
        index = 0;
    }
    else {
        return 0;
    }

    std::shared_ptr<RF62X::rf627smart> scanner = list[index];
    std::shared_ptr<RF62X::hello_info> info = scanner->get_info();

    // Display info about the scanner from which the profile will be received
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Device information: "                      << std::endl;
    std::cout << "* Name  \t: "   << info->device_name()     << std::endl;
    std::cout << "* Serial\t: "   << info->serial_number()   << std::endl;
    std::cout << "* IP Addr\t: "  << info->ip_address()      << std::endl;
    std::cout << "-----------------------------------------" << std::endl;

    // Establish connection to the scanner
    bool is_connected = scanner->connect();

    if (is_connected)
    {
        std::cout << "Connection established successfully" << std::endl;

        // Create buffer with standard capacity
        ProfileBuffer buffer;
        // Set scanner for receive profiles logic
        buffer.setScanner(scanner);

        // Additional settings
        buffer.setOption(ProfileBufferOptions::REALTIME, false);
        buffer.setOption(ProfileBufferOptions::ZERO_POINTS, true);
        buffer.setOption(ProfileBufferOptions::LOSS_DETECTION, true);

        // Creating error handler (to display errors in the terminal)
        ErrorHandler errorHandler;
        // Creating profiles handler (to process the profile immediately
        // at the time of capture)
        ProfileHandler profileHandler;

        // Bind error handler
        buffer.setErrorHandler(
                    [&errorHandler](std::string info){
            return errorHandler.print(info);
        });

        // Bind profile handler
        buffer.setProfileHandler(
                    [&profileHandler](std::shared_ptr<RF62X::profile2D> profile){
            return profileHandler.process(profile);
        });

        // Start of receiving profiles
        buffer.startCapturing();
        // Wait 3 seconds
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // Stop of receiving profiles
        buffer.stopCapturing();

        std::cout << "The number of profiles in the buffer after 3 seconds "
                     "of receiving: "<< buffer.getSize() << std::endl;

        int count = 0;
        while (auto profile = buffer.getBack()) {
            count++;
        }
        std::cout << count << " profiles were read from buffer" << std::endl;
    }else
    {
        std::cout << "Failed to connect to the scanner" << std::endl;
    }
}
```
