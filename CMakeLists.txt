cmake_minimum_required(VERSION 3.20)
project(lab2 CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options("-Wall" "-Wconversion" "-Wextra" "-Wpedantic")
endif()

add_executable(generator src/generator.cpp)
target_include_directories(generator PRIVATE include)
target_compile_definitions(generator PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

add_executable(lab2 src/main.cpp)
target_include_directories(lab2 PRIVATE include)
target_link_libraries(lab2 OpenMP::OpenMP_CXX)
target_compile_definitions(lab2 PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/input")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/output")