APPNAME=app
CFLAGS=-std=c17
LIBS=$(shell pkg-config --libs glfw3 glew libpng libjpeg)
SOURCES=$(shell ls -d ../shared/*.[ch]) $(shell ls *.[ch])

all:
	gcc $(CFLAGS) -o $(APPNAME) $(SOURCES) $(LIBS)
