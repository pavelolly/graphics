
set(SOURCES
    main.cpp

    gui/gui.cpp
    gui/gui.hpp

    geometry/geometry.cpp
    geometry/geometry.hpp
    geometry/bezier.cpp
    geometry/bezier.hpp
    geometry/polygon_animation.cpp
    geometry/polygon_animation.cpp
    
    scenes/point_dragger.cpp
    scenes/point_dragger.hpp
    
    scenes/scene.hpp
    scenes/scene_ellipses.cpp
    scenes/scene_ellipses.hpp
    scenes/scene_draw_polygons.cpp
    scenes/scene_draw_polygons.hpp
    scenes/scene_localization.cpp
    scenes/scene_localization.hpp
    scenes/scene_bezier_elementary.cpp
    scenes/scene_bezier_elementary.hpp
    scenes/scene_bezier.cpp
    scenes/scene_bezier.hpp
)

add_executable(main ${SOURCES})
if (MSVC)
    target_compile_options(main PUBLIC "/W3")
else()
    target_compile_options(main PUBLIC "-Wall" "-Wextra")
endif()

target_include_directories(main PRIVATE
                           ./
                           ../include/)

target_link_libraries(main PUBLIC raygui raylib)