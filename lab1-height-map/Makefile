APPNAME=app
CFLAGS=-std=c17
LIBS=$(shell pkg-config --libs glfw3 glew libpng libjpeg)
SHARED_SOURCES=bmp.h bmp.c file.h file.c image.h image.c model.c model.h shader.h shader.c shaderprogram.h shaderprogram.c util.h util.c
SOURCES=main.c main.h $(SHARED_SOURCES)

all:
	gcc $(CFLAGS) -o $(APPNAME) $(SOURCES) $(LIBS)