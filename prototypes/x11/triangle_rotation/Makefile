TARGET = glxtriangle

DEBUG_TARGET   = $(TARGET)_debug
RELEASE_TARGET = $(TARGET)

CC  = clang -c
CXX = clang++ -c
LD  = clang -fuse-ld=lld

CFLAGS_APPEND =

C_OBJS = glxtriangle.o
CXX_OBJS =

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lGLU -lGLEW -lX11

include ../../Makefile.common
