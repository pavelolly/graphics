
set(SOURCES
    main.cpp
    geometry.cpp geometry.hpp
    gui.cpp gui.hpp
    polygon_animation.cpp polygon_animation.cpp
    scene.hpp
    scene_ellipses.cpp scene_ellipses.hpp
    scene_draw_polygons.cpp scene_draw_polygons.hpp
    scene_localization.cpp scene_localization.hpp
    scene_bezier_elementary.cpp scene_bezier_elementary.hpp)

add_executable(main ${SOURCES})
if (MSVC)
    target_compile_options(main PUBLIC "/W3")
else()
    target_compile_options(main PUBLIC "-Wall" "-Wextra")
endif()

target_include_directories(main PUBLIC "../include")

target_link_libraries(main PUBLIC raygui raylib)