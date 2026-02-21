cmake_minimum_required(VERSION 3.20)
project(IT-Projekt-WS25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
find_package(LLVM REQUIRED CONFIG)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

if(TARGET LLVM)
    set(llvm_libs LLVM)
else()
    llvm_map_components_to_libnames(llvm_libs support core irreader)
endif()

file(GLOB_RECURSE LOGIC_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM LOGIC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(project_configs INTERFACE)
target_include_directories(project_configs INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/test
    ${LLVM_INCLUDE_DIRS}
)

# 1. Target: app
add_executable(app src/main.cpp ${LOGIC_SOURCES})
target_link_libraries(app PRIVATE project_configs ${llvm_libs})
target_precompile_headers(app PUBLIC 
    "src/core/Typedef.h"
    <vector>
    <string>
    <memory>
    <llvm/IR/IRBuilder.h>
    <llvm/IR/Module.h>
)

# 2. Target: test
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "test/*.cpp")
add_executable(test ${TEST_SOURCES} ${LOGIC_SOURCES})
target_link_libraries(test PRIVATE project_configs ${llvm_libs})

target_precompile_headers(test REUSE_FROM app)