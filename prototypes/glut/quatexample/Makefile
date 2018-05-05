TARGET = quat

TARGET_DEBUG = $(TARGET)_debug

CC  = clang -c
CXX = clang++ -c
LD  = clang

CFLAGS_RELEASE   = -Wall -O2
CXXFLAGS_RELEASE = -Wall -O2

CFLAGS_DEBUG   = -g -Wall -O0
CXXFLAGS_DEBUG = -g -Wall -O0

C_OBJS = main.co quat.co
CXX_OBJS =

C_OBJS_RELEASE   = ${C_OBJS:.co=.rco}
CXX_OBJS_RELEASE = ${CXX_OBJS:.cppo=.rcppo}

OBJS_RELEASE = $(C_OBJS_RELEASE) $(CXX_OBJS_RELEASE)

C_OBJS_DEBUG   = ${C_OBJS:.co=.dco}
CXX_OBJS_DEBUG = ${CXX_OBJS:.cppo=.dcppo}

OBJS_DEBUG = $(C_OBJS_DEBUG) $(CXX_OBJS_DEBUG)

SRCS = ${C_OBJS:.co=.c}

LIBS = -lm -lGL -lGLU -lglut

%.rco : %.c
	$(CC) $(CFLAGS_RELEASE) $< -o $@

%.rcppo : %.cpp
	$(CXX) $(CXXFLAGS_RELEASE) $< -o $@

%.dco : %.c
	$(CC) $(CFLAGS_DEBUG) $< -o $@

%.dcppo : %.cpp
	$(CXX) $(CXXFLAGS_DEBUG) $< -o $@

all: release

release : $(TARGET)

debug   : $(TARGET_DEBUG)

$(TARGET) : $(OBJS_RELEASE)
	$(LD) $(LIBS) $^ -o $@

$(TARGET_DEBUG) :  $(OBJS_DEBUG)
	$(LD) $(LIBS) $^ -o $@

clean:
	rm -rf $(TARGET) $(TARGET_DEBUG) *.rco *.rcppo *.dco *.dcppo *~ core


