# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(napi_call_function_exception)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../)

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(napi_call_function_exception SHARED napicallfunctionexception.cpp)
add_subdirectory(${NATIVERENDER_ROOT_PATH} aki)
target_link_libraries(napi_call_function_exception PUBLIC
    aki_jsbind)