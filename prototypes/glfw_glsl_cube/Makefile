TARGET = glfw_glsl_cube

OPENGL_INC_PATH  = -I/usr/local/cuda/samples/common/inc
OPENGL_INC_PATH += -I/usr/include/nvidia-352
OPENGL_LIB_PATH  = -L/usr/local/cuda/samples/common/lib/linux/x86_64
OPENGL_LIB_PATH += -L/usr/lib/nvidia-352

CC  = clang -c
CXX = clang++ -c
LD  = clang -v
#CC  = gcc -c
#CXX = g++ -c
#LD  = gcc

CFLAGS = $(OPENGL_INC_PATH)
CXXFLAGS = $(OPENGL_INC_PATH)

C_OBJS   = main.o shaderprogramset.o
CXX_OBJS =

VERT_OBJS = cube.vo
FRAG_OBJS =

LIBS = -lm -lGL -lGLU -lGLEW -lglfw -lstdc++

RELEASE_TARGET=$(TARGET)
DEBUG_TARGET=$(TARGET)_debug

include ../Makefile.common
