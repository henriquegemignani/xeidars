CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(xeidars CXX C)
MESSAGE("== XEIDARS ==")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# FindUGDK
add_subdirectory(externals/ugdk)
include_directories(${UGDK_EXTERNAL_INCLUDE_DIRS})
set(UGDK_EXTERNAL_LIBRARIES libugdk)
add_definitions("-DEMBBEDED_UGDK")

add_subdirectory(src) # The game binary.
