TARGET = quat

DEBUG_TARGET   = $(TARGET)_debug
RELEASE_TARGET = $(TARGET)

CC  = clang -c
CXX = clang++ -c
LD  = clang

CFLAGS_APPEND = `pkg-config --cflags gtk+-3.0`

C_OBJS = main.o
CXX_OBJS =

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lGLU -lglut `pkg-config --libs gtk+-3.0`

include ../../Makefile.common
