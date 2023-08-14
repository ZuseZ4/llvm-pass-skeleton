cmake_minimum_required(VERSION 3.13)
project(Skeleton)

# support C++14 features used by LLVM 10.0.0
set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using LLVMcmake in: ${LLVM_CMAKE_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

add_subdirectory(skeleton)  # Use your pass name here.

include(AddLLVM)

add_llvm_pass_plugin(SkeletonPass skeleton/Skeleton.cpp)
