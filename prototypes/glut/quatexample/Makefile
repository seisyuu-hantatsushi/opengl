TARGET = quat

CC  = clang -c
CXX = clang++ -c
LD  = clang

CFLAGS = -Wall -O2
CXXFLAGS = -Wall -O2

C_OBJS = main.co quat.co
CXX_OBJS =

OBJS = $(C_OBJS) $(CXX_OBJS)

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lGLU -lglut

%.co : %.c
	$(CC) $(CFLAGS) $< -o $@

%.cppo : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) $(LIBS) $^ -o $@

clean:
	rm -rf $(TARGET) $(OBJS) *~


