cmake_minimum_required(VERSION 3.13)  # CMake version check
project(yeelight_library)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 20)            # Enable c++14 standard

ADD_LIBRARY( yeelight-lib STATIC
             src/yeelight/bulbs_discover.cpp
)

include_directories(src/yeelight)

set(SOURCE_FILES src/example/main.cpp)
add_executable(example ${SOURCE_FILES})


TARGET_LINK_LIBRARIES( example
                       yeelight-lib )
