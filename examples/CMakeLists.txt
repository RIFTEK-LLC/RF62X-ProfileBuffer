cmake_minimum_required(VERSION 3.13)



################################################################################
## PROJECT
## name and version
################################################################################
project(examples LANGUAGES CXX)



################################################################################
## CONFIGURATION
## project configuration
################################################################################
SET(${PARENT}_PROFILE_BUFFER_DEMO_APP                    ON CACHE BOOL "" FORCE)



################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the configuration
################################################################################
if (${PARENT}_PROFILE_BUFFER_DEMO_APP)
    add_subdirectory(ProfileBufferDemoApp)
endif()
