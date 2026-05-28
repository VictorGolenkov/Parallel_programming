cmake_minimum_required(VERSION 3.20)
project(lab3 CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(MPI REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options("-Wall" "-Wconversion" "-Wextra" "-Wpedantic")
endif()

add_executable(generator src/generator.cpp)
target_include_directories(generator PRIVATE include)
target_compile_definitions(generator PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

add_executable(lab3 src/main.cpp include/matrix.hpp)
target_include_directories(lab3 PRIVATE include ${MPI_CXX_INCLUDE_PATH})
target_link_libraries(lab3 ${MPI_CXX_LIBRARIES})
target_compile_definitions(lab3 PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/input")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/output")

if(WIN32)
    set(MPI_CXX_INCLUDE_PATH "C:/Program Files (x86)/Microsoft SDKs/MPI/Include")
    set(MPI_CXX_LIBRARIES "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/msmpi.lib")
endif()

