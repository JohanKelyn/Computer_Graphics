cmake_minimum_required(VERSION 4.0)
project(raytracer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set( SOURCE_ONE_WEEKEND 
main.cpp
./header_files/color.cpp
./header_files/sphere.cpp
./header_files/ray.cpp
./header_files/hittable_list.cpp
./header_files/interval.cpp
./header_files/camera.cpp
./header_files/material.cpp
./header_files/aabb.cpp
./header_files/bvh.cpp
./header_files/texture.cpp
./header_files/rtw_stb_image.cpp
./header_files/perlin.cpp
./header_files/quad.cpp
)

add_executable(raytracer ${SOURCE_ONE_WEEKEND} )

target_include_directories(raytracer 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../ext/eigen/"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../ext/stb/"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/header_files/"
)

