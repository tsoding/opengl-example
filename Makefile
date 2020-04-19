PKGS=glfw3 gl libpng
CFLAGS=$(shell pkg-config --cflags $(PKGS)) -Wall -Werror
LIBS=$(shell pkg-config --libs $(PKGS))

opengl-example: main.c
	$(CC) $(CFLAGS) -o opengl-example main.c $(LIBS)
