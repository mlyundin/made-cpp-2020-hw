cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

set(project_name list)

project(${project_name})
file(GLOB SRC_FILES src/*.cpp)
file(GLOB TEST_FILES test/*.cpp)
add_executable(${project_name} ${SRC_FILES} ${TEST_FILES})
target_include_directories(${project_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")