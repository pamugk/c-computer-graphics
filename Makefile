CFLAGS=-std=c2x -march=native -O1
LIBS=$(shell pkg-config --libs alure glfw3 glew libpng libjpeg) -lm

all:
	gcc $(CFLAGS) -o app shared/*.[ch] application/*.[ch] $(LIBS)
clean:
	rm shared/*.gch application/*.gch
