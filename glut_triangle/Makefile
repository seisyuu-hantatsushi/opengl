TARGET = glut_triangle

CC  = clang -c 
CXX = clang++ -c
LD  = clang

CXXFLAGS =

C_OBJS   = main.co
CXX_OBJS = 

OBJS = $(C_OBJS) $(CXX_OBJS)

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lglut

%.co : %.c
	$(CC) $(CFLAGS) $< -o $@ 

%.cppo : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ 

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) $(LIBS) $< -o $@

clean:
	rm -rf $(TARGET) *.co
