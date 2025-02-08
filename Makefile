RAYLIB_PATH = raylib/src

vpath libraylib.a ${RAYLIB_PATH}

ifeq (${OS},Windows_NT)
    MAIN = main.exe
	LD_FLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32
else
    MAIN = main
	LD_FLAGS = -lraylib
endif

CC = g++
CFLAGS = --std=c++20 -Wall -Wextra
INCLUDE_PATH = -I ${RAYLIB_PATH} -I raygui/src
LD_PATH = -L ${RAYLIB_PATH}


${MAIN}: main.cpp libraylib.a
	${CC} -o $@ main.cpp ${INCLUDE_PATH} ${LD_PATH} ${LD_FLAGS}

libraylib.a:
	${MAKE} -C ${RAYLIB_PATH}