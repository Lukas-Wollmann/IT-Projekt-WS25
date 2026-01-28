cmake_minimum_required(VERSION 3.20)
project(IT-Projekt-WS25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")
set(APP_SRCS ${SRC_FILES} ${HEADER_FILES})

add_executable(app ${APP_SRCS})
include_directories(lib src test)

file(GLOB_RECURSE SRC_FILES_TEST "test/*.cpp")
file(GLOB_RECURSE HEADER_FILES_TEST "test/*.h")
set(TEST_SRCS ${APP_SRCS} ${SRC_FILES_TEST} ${HEADER_FILES_TEST})
list(FILTER TEST_SRCS EXCLUDE REGEX "src/main.cpp")

add_executable(test ${TEST_SRCS})

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(app ${llvm_libs})
target_link_libraries(test ${llvm_libs})