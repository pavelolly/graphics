RAYLIB_PATH = raylib/src

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


INCLUDE_PATH = -I ${RAYLIB_PATH} -I ./
LD_PATH = -L ${RAYLIB_PATH}

HEADERS = geometry.hpp gui.hpp log_macros.h scenes.hpp

${MAIN}: main.cpp ${HEADERS} libraylib.a raygui.o
	${CC} ${CFLAGS} ${INCLUDE_PATH} -DRAYGUI_VALUEBOX_MAX_CHARS=10 -o $@ main.cpp raygui.o ${LD_PATH} ${LD_FLAGS}

libraylib.a:
	${MAKE} -C ${RAYLIB_PATH}

raygui.o: raygui.h
	${CC} --std=c11 -O2 ${INCLUDE_PATH} -DRAYGUI_IMPLEMENTATION -DRAYGUI_VALUEBOX_MAX_CHARS=10 -o $@ -c -x c raygui.h ${LD_PATH} ${LD_FLAGS}