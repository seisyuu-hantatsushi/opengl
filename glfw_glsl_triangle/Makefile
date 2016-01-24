TARGET = glfw_glsl_triangle

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

C_OBJS   = main.co
CXX_OBJS = 

OBJS = $(C_OBJS) $(CXX_OBJS)

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lglfw /usr/local/cuda/samples/common/lib/linux/x86_64/libGLEW.a

LDFLAGS = $(OPENGL_LIB_PATH) $(LIBS)

%.co : %.c
	$(CC) $(CFLAGS) $< -o $@ 

%.cppo : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ 

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) $< $(LDFLAGS) -o $@

clean:
	rm -rf $(TARGET) *.co
