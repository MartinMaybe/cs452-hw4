prog=wam

ccflags=-pthread
ldflags=-pthread -lX11 -lfltk

ifeq ($(shell uname),Darwin)
ccflags += -I/opt/X11/include $(shell fltk-config --cxxflags)
ldflags  = -pthread -L/opt/X11/lib -lX11 $(shell fltk-config --ldflags)
endif

ld=g++

include ../GNUmakefile