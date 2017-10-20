
NAME = test

RELEASE_TARGET = $(NAME)

TARGETS = $(RELEASE_TARGET)

INCDIRS = -I../../library/include

OBJS_CXX = main.o

RELEASE_OBJS_C = ${OBJS_C:.o=.rco}
RELEASE_OBJS_CXX = ${OBJS_CXX:.o=.rcppo}

DEBUG_OBJS_C   = ${OBJS_C:.o=.dco}
DEBUG_OBJS_CXX = ${OBJS_CXX:.o=.dcppo}

LIBS = -lstdc++ -lm

include ../Makefile.common
