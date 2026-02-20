cmake_minimum_required(VERSION 3.11)

# Project name
project(raylibvsrg)

#set your c++ standard here
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#generate a compile commands.json if you use clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

option(TRACY_ENABLE "" ON)
option(TRACY_ON_DEMAND "" ON)
FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)


FetchContent_MakeAvailable(raylib tracy)
message(STATUS "tracy source dir = ${tracy_SOURCE_DIR}")
message(STATUS "tracy binary dir = ${tracy_BINARY_DIR}")
add_subdirectory(${tracy_SOURCE_DIR}/profiler tracy)

file(GLOB_RECURSE SOURCES
                "src/*.cpp"
                "src/*.h"
)

# PROJECT
include_directories(src/)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(raylibvsrg PRIVATE 
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(${PROJECT_NAME}  PUBLIC TRACY_ENABLE)
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} TracyClient)