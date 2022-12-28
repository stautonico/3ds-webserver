cmake_minimum_required(VERSION 3.1)
project(3ds-webserver)

set(EXECUTABLE 3ds-webserver)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Fleet

option(TARGET_PLATFORM "TARGET_PLATFORM" LINUX)

# Add shared sources
set(SHARED_SOURCES src/server/server.cpp src/server/request.cpp)

# Platform specific sources
if("${TARGET_PLATFORM}" STREQUAL "3DS")
    # 3DS sources
    set(PLATFORM_SOURCES src/interface/3ds/interface.cpp)
else() # Linux sources
    set(PLATFORM_SOURCES src/interface/linux/interface.cpp)
endif()

add_executable(${EXECUTABLE} src/main.cpp ${SHARED_SOURCES} ${PLATFORM_SOURCES})


if("${TARGET_PLATFORM}" STREQUAL "3DS")
    # 3DS options
    message("Building for 3DS...")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DKA_SUGGESTED_C_FLAGS}") # Use the devkitArm suggested flags. This is not mandatory.
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) # Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
    include(Tools3DS) # Include all the macros and tools needed for 3ds development.
    find_package(CTRULIB REQUIRED) # Look for ctrulib
    target_link_libraries(${EXECUTABLE} 3ds::ctrulib)
    target_link_libraries(${EXECUTABLE} m)
    add_3dsx_target(${EXECUTABLE})
    add_definitions(-DIS_3DS)
else() # Default to linux build
    # Linux specific options
    message("Building for Linux...")
    add_definitions(-DIS_LINUX)
endif()
