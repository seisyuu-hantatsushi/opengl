TARGET = gtktriangle

DEBUG_TARGET   = $(TARGET)_debug
RELEASE_TARGET = $(TARGET)

CC  = clang -c
CXX = clang++ -c
LD  = clang -fuse-ld=lld

CFLAGS_APPEND = `pkg-config --cflags gtk+-3.0`

C_OBJS = main.o
CXX_OBJS =

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lGLU -lGLEW `pkg-config --libs gtk+-3.0`

include ../../Makefile.common
