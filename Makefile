CFLAGS=-std=c17
LIBS=$(shell pkg-config --libs glfw3 glew libpng libjpeg) -lm
SHARED_SOURCES=$(shell ls -d shared/*.[ch])

all: lab1-height-map lab2-textures

lab1-height-map:
	gcc $(CFLAGS) -o lab1-height-map/app $(SHARED_SOURCES) $(shell ls lab1-height-map/*.[ch]) $(LIBS)

lab2-textures:
	gcc $(CFLAGS) -o lab2-textures/app $(SHARED_SOURCES) $(shell ls lab2-textures/*.[ch]) $(LIBS)
