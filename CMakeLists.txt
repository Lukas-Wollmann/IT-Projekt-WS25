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
llvm_map_components_to_libnames(llvm_libs support core irreader)

include_directories(lib src test)

file(GLOB_RECURSE SRC_CPP CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE SRC_HEADER CONFIGURE_DEPENDS "src/*.h")
set(SRC_FILES ${SRC_CPP} ${SRC_HEADER})

add_library(logic STATIC ${SRC_FILES})

add_executable(app src/main.cpp)
target_link_libraries(app logic ${llvm_libs})

file(GLOB_RECURSE TEST_CPP CONFIGURE_DEPENDS "test/*.cpp")
file(GLOB_RECURSE TEST_HEADER CONFIGURE_DEPENDS "test/*.h")
set(TEST_FILES ${TEST_CPP} ${TEST_HEADER})

add_executable(test ${TEST_FILES})
target_link_libraries(test logic ${llvm_libs})