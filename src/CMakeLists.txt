
set(SOURCES
    main.cpp
    geometry.cpp geometry.hpp
    gui.cpp gui.hpp
    polygon_animation.cpp polygon_animation.cpp
    scene.hpp scenes.hpp
    scene_ellipses.cpp scene_ellipses.hpp
    scene_draw_polygons.cpp scene_draw_polygons.hpp
    scene_localization.cpp scene_localization.hpp)

add_executable(main ${SOURCES})
if (MSVC)
    target_compile_options(main PUBLIC "/W3")
else()
    target_compile_options(main PUBLIC "-Wall" "-Wextra")
endif()

target_include_directories(main PUBLIC "../include")

target_link_libraries(main PUBLIC raygui raylib)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND NOT MSVC)
    target_link_options(main PUBLIC "-lopengl32" "-lgdi32" "-lwinmm" "-lshell32")
endif()