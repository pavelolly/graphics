RAYLIB_PATH = raylib/src
RAYGUI_PATH = raygui/src

vpath libraylib.a ${RAYLIB_PATH}

ifeq (${OS},Windows_NT)
    MAIN = main.exe
	LD_FLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32
else
    MAIN = main
	LD_FLAGS = -lraylib
endif

CC = g++
CFLAGS = --std=c++20 -Wall -Wextra -ggdb

# TODO: compile below modules separately to aviod turning off warnings
# for compiling raymath
CFLAGS += -Wno-missing-field-initializers
# for compiling raygui
CFLAGS += -Wno-unused-parameter -Wno-enum-compare


INCLUDE_PATH = -I ${RAYLIB_PATH} -I ${RAYGUI_PATH}
LD_PATH = -L ${RAYLIB_PATH}

HEADERS = geometry.h gui.h log_macros.h scenes.h

${MAIN}: main.cpp ${HEADERS} libraylib.a
	${CC} ${CFLAGS} -o $@ main.cpp ${INCLUDE_PATH} ${LD_PATH} ${LD_FLAGS}

libraylib.a:
	${MAKE} -C ${RAYLIB_PATH}