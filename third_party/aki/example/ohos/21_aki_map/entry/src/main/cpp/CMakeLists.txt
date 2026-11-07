# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(aki_map)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../)

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(aki_map SHARED hello.cpp)
add_subdirectory(${NATIVERENDER_ROOT_PATH} aki)
target_link_libraries(aki_map PUBLIC aki_jsbind)
target_link_libraries(aki_map PUBLIC libhilog_ndk.z.so)