cmake_minimum_required(VERSION 3.20)
project(lab4_cuda CXX CUDA)
set(CMAKE_CXX_STANDARD 20)

find_package(CUDA REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options("-Wall" "-Wconversion" "-Wextra" "-Wpedantic")
endif()

# Generator (чистый C++)
add_executable(generator src/generator.cpp)
target_include_directories(generator PRIVATE include)
target_compile_definitions(generator PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

# CUDA-версия
add_executable(lab4 src/main.cpp src/matrix_cuda.cu)
target_include_directories(lab4 PRIVATE include ${CUDA_INCLUDE_DIRS})
target_link_libraries(lab4 ${CUDA_LIBRARIES} cudart)
target_compile_definitions(lab4 PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/input")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/output")