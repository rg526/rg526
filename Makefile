INCDIR=-I/usr/include/freetype2 -Ies
CFLAGS=-Wall -Wextra -Wno-unused-parameter
LIBS=-lGLESv2 -lEGL -lX11 -lm -pthread -lgpiod -lfreetype

all: rg526

run: rg526
	./rg526

bin/%.o: %.c
	gcc $^ -c -o $@ $(CFLAGS) $(INCDIR)

bin/%.o: es/%.c
	gcc $^ -c -o $@ $(INCDIR)

rg526: bin/state.o bin/playmode.o bin/image.o bin/text.o bin/music.o bin/note.o bin/input.o bin/gpio.o bin/mat.o bin/vec.o bin/model.o bin/modedisplay.o bin/pausemode.o bin/homemode.o bin/selectmode.o bin/endmode.o bin/esShader.o bin/esUtil.o bin/esUtil_X11.o
	gcc $^ -o $@ $(CFLAGS) $(INCDIR) $(LIBS)

clean:
	rm -f bin/*.o rg526
