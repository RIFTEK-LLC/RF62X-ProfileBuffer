cmake_minimum_required(VERSION 3.13)



################################################################################
## 3RD-PARTY
## dependencies for the project
################################################################################
project(3rdparty LANGUAGES CXX)



################################################################################
## SETTINGS
## basic 3rd-party settings before use
################################################################################
# To inherit the top-level architecture when the project is used as a submodule.
SET(PARENT ${PARENT}_3RDPARTY_)
# Disable self-overwriting of parameters inside included subdirectories.
SET(${PARENT}_SUBMODULE_CACHE_OVERWRITE OFF CACHE BOOL "" FORCE)



################################################################################
## CONFIGURATION
## 3rd-party submodules configuration
################################################################################
SET(${PARENT}_RF62X_SDK_SUBMODULE                     ON  CACHE BOOL   "" FORCE)
if (${PARENT}_RF62X_SDK_SUBMODULE)
    SET(${PARENT}_WRAPPER_LANGUAGE                  "C++" CACHE STRING "" FORCE)
    SET(${PARENT}_RF62X_SDK_EXAMPLES                 OFF  CACHE BOOL   "" FORCE)
endif()


################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the 3rd-party configuration
################################################################################
if (${PARENT}_RF62X_SDK_SUBMODULE)
    add_subdirectory(RF62X-SDK)
endif()
