cmake_minimum_required(VERSION 3.20)

project(packOpener VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

find_package(raylib QUIET)
if(NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)
endif()

find_path(RAYLIB_CPP_INCLUDE_DIR NAMES raylib-cpp.hpp raylib.hpp)
if(NOT RAYLIB_CPP_INCLUDE_DIR)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v5.5.0
    )
    FetchContent_MakeAvailable(raylib_cpp)
    set(RAYLIB_CPP_INCLUDE_DIR ${raylib_cpp_SOURCE_DIR}/include)
endif()

add_executable(packOpener
    src/Card.cpp
    src/CardDatabase.cpp
    src/GameState.cpp
    src/GuiController.cpp
    src/Inventory.cpp
    src/Pack.cpp
    src/PackDatabase.cpp
    src/PackHistoryList.cpp
    src/PackOpeningService.cpp
    src/SaveService.cpp
    src/Utils.cpp
    src/main.cpp
)

target_include_directories(packOpener PRIVATE include ${RAYLIB_CPP_INCLUDE_DIR})
target_link_libraries(packOpener PRIVATE raylib)

if(TARGET raylib_cpp)
    target_link_libraries(packOpener PRIVATE raylib_cpp)
endif()

if(APPLE)
    target_link_libraries(packOpener PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()
