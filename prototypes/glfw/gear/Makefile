TARGET = glfw_gears

#CC  = clang -c 
#CXX = clang++ -c
#LD  = clang
CC = gcc -c
CXX = g++ -c
LD = gcc


CXXFLAGS =

C_OBJS   = main.co
CXX_OBJS = 

OBJS = $(C_OBJS) $(CXX_OBJS)

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lglfw

%.co : %.c
	$(CC) $(CFLAGS) $< -o $@ 

%.cppo : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ 

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) $< $(LIBS) -o $@

clean:
	rm -rf $(TARGET) *.co
