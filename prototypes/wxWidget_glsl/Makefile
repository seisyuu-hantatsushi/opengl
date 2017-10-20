CC  = clang -c
CXX = clang++ -c
LD  = clang -v

OBJS_DIR=./objs
SRCS_DIR=./

NAME=test

C_OBJS =
CPP_OBJS = $(OBJS_DIR)/main.o

RELEASE_OUT=$(NAME)
DEBUG_OUT=$(NAME)_debug

CPPFLAGS=`wx-config --cflags`

LDFLAGS=`wx-config --libs --gl-libs` -lstdc++ -lGL -lGLU

C_REL_OBJS   = ${C_OBJS:.o=.rco}
CPP_REL_OBJS = ${CPP_OBJS:.o=.rcppo}

RELEASE_OBJS = $(C_REL_OBJS) $(CPP_REL_OBJS)

$(OBJS_DIR)/%.rcppo: $(SRCS_DIR)/%.cpp
	if [ ! -d $(OBJS_DIR) ]; then \
		mkdir -p $(OBJS_DIR); \
	fi
	$(CXX) $(CPPFLAGS) $< -o $@

release: $(RELEASE_OUT)

debug: $(DEBUG_OUT)

all: release

$(RELEASE_OUT): $(RELEASE_OBJS)
	$(LD) $< $(LDFLAGS) -o $@ 

clean:
	rm -rf $(SRCS_DIR)/*~ $(OBJS_DIR) $(RELEASE_OUT) $(DEBUG_OUT)
