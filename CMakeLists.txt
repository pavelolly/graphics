cmake_minimum_required(VERSION 3.25)
project(graphics C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)


set(CMAKE_BUILD_TYPE Release)
add_subdirectory(raylib)

add_library(raygui STATIC ./include/raygui.h ./include/raygui.c)
set_target_properties(raygui PROPERTIES LINKER_LANGUAGE C)
if (MSVC)
    target_compile_definitions(raygui PRIVATE "_CRT_SECURE_NO_WARNINGS")
endif()
target_include_directories(raygui PRIVATE ./include)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(src)