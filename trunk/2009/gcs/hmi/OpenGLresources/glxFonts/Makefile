XLIBS= -L/usr/X11R6/lib -lXmu -lXt -lSM -lICE -lXext -lX11 -lXi -lXext -lX11
GL_LIBS = -lglut -lMesaGLU -lMesaGL -lm
LDFLAGS=  -Wl,-rpath,/usr/X11R6/lib

OBJECTS= \
  glxfont.o

glxfont: $(OBJECTS)
	gcc $(CFLAGS) $(XLIBS) $(GL_LIBS) $(LDFLAGS) -o $@ $(OBJECTS)

glxfont.o: glxfont.c
	gcc $(CFLAGS) -c $<

