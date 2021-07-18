APPNAME=app
CFLAGS=$(shell pkg-config --cflags glfw3 gl) -std=c17
LIBS=$(shell pkg-config --libs glfw3 glew)
SOURCES=main.c main.h shader.h shader.c  shaderprogram.h shaderprogram.c model.c model.h

all:
	gcc $(CFLAGS) -o $(APPNAME) $(SOURCES) $(LIBS)