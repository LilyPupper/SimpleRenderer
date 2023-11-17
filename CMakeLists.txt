cmake_minimum_required(VERSION 3.2)
set(CMAKE_SUPPRESS_REGENERATION true)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SimpleRenderer)
cmake_policy(SET CMP0091 NEW)

project(SimpleRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

add_definitions(-DUNICODE -D_UNICODE)

add_subdirectory(vendor)
add_subdirectory(Tests)
add_subdirectory(SimpleRenderer)
