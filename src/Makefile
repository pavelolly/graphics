RAYLIB_PATH = ../raylib/src

vpath libraylib.a ${RAYLIB_PATH}

CXX = g++
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
		  scene_ellipses.o \
		  scene_draw_polygons.o \
		  scene_localization.o \
		  polygon_animation.o \
		  raygui.o

${MAIN}: main.cpp ${OBJECTS} libraylib.a
	${CXX} ${CXXFLAGS} -o $@ ${INCLUDE_PATH} $< ${OBJECTS} ${LD_PATH} ${LD_FLAGS}

%.o: %.cpp
	${CXX} ${CXXFLAGS} -c -o $@ ${INCLUDE_PATH} $< ${LD_PATH} ${LD_FLAGS}

geometry.o: geometry.cpp geometry.hpp
gui.o: gui.cpp gui.hpp
scene_ellipses.o: scene_ellipses.cpp scenes.hpp
scene_draw_polygons.o: scene_draw_polygons.cpp scenes.hpp
scene_localization.o: scene_localization.cpp scenes.hpp
polygon_animation.o: polygon_animation.cpp scenes.hpp

raygui.o: ../include/raygui.h
	${CXX} --std=c11 -O2 ${INCLUDE_PATH} -DRAYGUI_IMPLEMENTATION -DRAYGUI_VALUEBOX_MAX_CHARS=10 -o $@ -c -x c $< ${LD_PATH} ${LD_FLAGS}

libraylib.a:
	${MAKE} -C ${RAYLIB_PATH}