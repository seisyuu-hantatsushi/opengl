TARGET = quat

DEBUG_TARGET   = $(TARGET)_debug
RELEASE_TARGET = $(TARGET)

CC  = clang -c
CXX = clang++ -c
LD  = clang

C_OBJS = main.o quat.o
CXX_OBJS =

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lGLU -lglut

include ../../Makefile.common
