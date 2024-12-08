cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(App)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

target_include_directories(
    ${CMAKE_PROJECT_NAME} PUBLIC ./../../../ext/GLEW/include/
    ${CMAKE_PROJECT_NAME} PUBLIC ./../../../ext/GLFW/include/
    ${CMAKE_PROJECT_NAME} PUBLIC ./../../../ext/ASSIMP/include/
    ${CMAKE_PROJECT_NAME} PUBLIC ./../../../ext/glm/
    ${CMAKE_PROJECT_NAME} PUBLIC ./../../../ext/stb/
    ${CMAKE_PROJECT_NAME} PUBLIC ./../../HeaderFiles/
)

target_link_directories(
    ${CMAKE_PROJECT_NAME} PUBLIC ./../../../ext/GLEW/build/lib/
    ${CMAKE_PROJECT_NAME} PUBLIC ./../../../ext/GLFW/src/
)

target_link_libraries(${CMAKE_PROJECT_NAME} glfw3 GLEW OpenGL assimp)