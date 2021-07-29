CFLAGS=-std=c17
LIBS=$(shell pkg-config --libs glfw3 glew libpng libjpeg) -lm
SOURCES=$(shell ls -d shared/*.[ch]) $(shell ls application/*.[ch])

all:
	gcc $(CFLAGS) -o application/app $(SOURCES) $(LIBS)
