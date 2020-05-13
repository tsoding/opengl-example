PKGS=glfw3 gl
CFLAGS=$(shell pkg-config --cflags $(PKGS)) -Wall -Werror
LIBS=$(shell pkg-config --libs $(PKGS)) -lm

opengl-example: main.c
	$(CC) $(CFLAGS) -o opengl-example main.c $(LIBS)
