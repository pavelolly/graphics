RAYLIB_PATH = raylib/src
RAYGUI_PATH = raygui/src

vpath libraylib.a ${RAYLIB_PATH}

CC = g++
CFLAGS = --std=c++20 -Wall -Wextra -ggdb

ifeq (${OS},Windows_NT)
    MAIN = main.exe
    LD_FLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32
else
    MAIN = main
    CFLAGS += -fPIC # -fsanitize=address
    LD_FLAGS = -lraylib -lm
endif

# TODO: compile below modules separately to aviod turning off warnings
# for compiling raymath
CFLAGS += -Wno-missing-field-initializers
# for compiling raygui
CFLAGS += -Wno-unused-parameter -Wno-enum-compare


INCLUDE_PATH = -I ${RAYLIB_PATH} -I ${RAYGUI_PATH}
LD_PATH = -L ${RAYLIB_PATH}

HEADERS = geometry.hpp gui.hpp log_macros.h scenes.hpp

${MAIN}: main.cpp ${HEADERS} libraylib.a
	${CC} ${CFLAGS} -o $@ main.cpp ${INCLUDE_PATH} ${LD_PATH} ${LD_FLAGS}

libraylib.a:
	${MAKE} -C ${RAYLIB_PATH}