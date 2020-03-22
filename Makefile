PKGS=glfw3 gl libpng
CFLAGS=$(shell pkg-config --cflags $(PKGS))
LIBS=$(shell pkg-config --libs $(PKGS))

opengl: main.c
	$(CC) $(CFLAGS) -o opengl main.c $(LIBS)
