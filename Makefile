CFLAGS=-std=c2x -g
LIBS=$(shell pkg-config --libs alure glfw3 glew libpng libjpeg) -lm

all:
	clang $(CFLAGS) shared/*.[ch] application/*.[ch] $(LIBS)
