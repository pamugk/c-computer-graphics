CFLAGS=-std=c17
LIBS=$(shell pkg-config --libs glfw3 glew libpng libjpeg) -lm
SHARED_SOURCES=$(shell ls -d shared/*.[ch])

all: lab1-height-map

lab1-height-map:
	gcc $(CFLAGS) -o lab1-height-map/app $(SHARED_SOURCES) $(shell ls lab1-height-map/*.[ch]) $(LIBS)
