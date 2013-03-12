CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(xeidars CXX C)
MESSAGE("== XEIDARS ==")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# FindUGDK
set(UGDK_CREATE_BINDINGS OFF)
set(UGDK_BUILD_TESTS OFF)
add_subdirectory(externals/ugdk)
include_directories(${UGDK_EXTERNAL_INCLUDE_DIRS})
set(UGDK_EXTERNAL_LIBRARIES libugdk)
add_definitions("-DEMBBEDED_UGDK")

set(GAME_SRCS "main.cc")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
if (UNIX OR MINGW)
    # se for o g++ para *nix ou o MinGW para Windows:
    add_definitions(-Wall -ansi -U_FORTIFY_SOURCE -msse2)
elseif (MSVC) 
    add_definitions(/D_CRT_SECURE_NO_WARNINGS /W4 /wd4100 /wd4127 /wd4201 /wd4211 /wd4250 /wd4706 /arch:SSE2 /fp:fast /MP)
endif ()

add_executable(xeidars ${GAME_SRCS})
target_link_libraries(xeidars ${UGDK_EXTERNAL_LIBRARIES})