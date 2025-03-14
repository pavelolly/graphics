cmake_minimum_required(VERSION 3.25)
project(graphics C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_subdirectory(raylib)

add_library(raygui STATIC ./include/raygui.h ./include/raygui.c)
if (MSVC)
    target_compile_definitions(raygui PRIVATE "_CRT_SECURE_NO_WARNINGS")
endif()
target_include_directories(raygui PRIVATE ./include)

add_subdirectory(src)