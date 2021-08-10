CFLAGS=-std=c2x -g
LIBS=$(shell pkg-config --libs alure glfw3 glew libpng libjpeg) -lm

all:
	gcc $(CFLAGS) -o app ../shared/*.[ch] *.[ch] $(LIBS)
clean:
	rm ../shared/*.gch *.gch
