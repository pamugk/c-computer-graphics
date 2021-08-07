APPNAME=app
CFLAGS=-std=c17 -g
LIBS=$(shell pkg-config --libs alure glfw3 glew libpng libjpeg) -lm
SOURCES=$(shell ls -d ../shared/*.[ch]) $(shell ls *.[ch])

all:
	gcc $(CFLAGS) -o $(APPNAME) $(SOURCES) $(LIBS)
