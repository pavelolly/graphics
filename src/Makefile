RAYLIB_PATH = ../raylib/src

vpath libraylib.a ${RAYLIB_PATH}

CXX = g++
CC  = gcc
CXXFLAGS = --std=c++20 -Wall -Wextra -ggdb

ifeq (${OS},Windows_NT)
    MAIN = main.exe
    LD_FLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32
else
    MAIN = main
    CXXFLAGS += -fPIC # -fsanitize=address
    LD_FLAGS = -lraylib -lm
endif

INCLUDE_PATH = -I ../include
LD_PATH = -L ${RAYLIB_PATH}

OBJECTS = geometry.o \
          gui.o \
          polygon_animation.o \
          scene_ellipses.o \
          scene_draw_polygons.o \
          scene_localization.o \
          raygui.o

${MAIN}: main.cpp ${OBJECTS} libraylib.a
	${CXX} ${CXXFLAGS} -o $@ ${INCLUDE_PATH} $< ${OBJECTS} ${LD_PATH} ${LD_FLAGS}

%.o: %.cpp
	${CXX} ${CXXFLAGS} -c -o $@ ${INCLUDE_PATH} $<

geometry.o: geometry.cpp geometry.hpp
gui.o: gui.cpp gui.hpp
polygon_animation.o: polygon_animation.cpp polygon_animation.hpp
scene_ellipses.o: scene_ellipses.cpp scene_ellipses.hpp scene.hpp
scene_draw_polygons.o: scene_draw_polygons.cpp scene_draw_polygons.hpp scene.hpp
scene_localization.o: scene_localization.cpp scene_localization.hpp scene.hpp
# scene_elementary_bezier.o: scene_elementary_bezier.cpp scene_elementary_bezier.hpp scene.hpp

raygui.o: ../include/raygui.h
	${CC} --std=c11 -O2 ${INCLUDE_PATH} -DRAYGUI_IMPLEMENTATION -DRAYGUI_VALUEBOX_MAX_CHARS=10 -o $@ -c -x c $<

libraylib.a:
	${MAKE} -C ${RAYLIB_PATH}

.PHONY: clean

clean:
	${RM} *.exe *.o