cmake_minimum_required(VERSION 3.10)
project(file_storage CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags for optimization and warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Create executable named 'code' as required
add_executable(code main.cpp)

# If we have additional source files
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/storage.cpp")
    target_sources(code PRIVATE storage.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/storage.h")
    target_include_directories(code PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()