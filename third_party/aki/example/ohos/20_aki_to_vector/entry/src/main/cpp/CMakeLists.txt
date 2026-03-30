# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(21_aki_to_vector)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../)

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(aki_to_vector SHARED hello.cpp)
add_subdirectory(${NATIVERENDER_ROOT_PATH} aki)
target_link_libraries(aki_to_vector PUBLIC aki_jsbind)