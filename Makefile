all: rg526

run: rg526
	./rg526

bin/state.o: state.c
	gcc state.c -c -o bin/state.o -Wall -Ies

bin/gameplay.o: gameplay.c
	gcc gameplay.c -c -o bin/gameplay.o -Wall -Ies

bin/note.o: note.c 
	gcc note.c -c -o bin/note.o -Wall -Ies 

bin/input.o: input.c
	gcc input.c -c -o bin/input.o -Wall -Ies

bin/gpio.o: gpio.c
	gcc gpio.c -c -o bin/gpio.o -Wall -Ies

bin/music.o: music.c
	gcc music.c -c -o bin/music.o -Wall -Ies

bin/mat.o: mat.c
	gcc mat.c -c -o bin/mat.o -Wall -Ies

bin/vec.o: vec.c
	gcc vec.c -c -o bin/vec.o -Wall -Ies

bin/model.o: model.c
	gcc model.c -c -o bin/model.o -Wall -Ies

bin/esShader.o: es/esShader.c
	gcc es/esShader.c -c -o bin/esShader.o 

bin/esUtil.o: es/esUtil.c
	gcc es/esUtil.c -c -o bin/esUtil.o

bin/esUtil_X11.o: es/esUtil_X11.c
	gcc es/esUtil_X11.c -c -o bin/esUtil_X11.o

rg526: bin/state.o bin/gameplay.o bin/music.o bin/note.o bin/input.o bin/gpio.o bin/mat.o bin/vec.o bin/model.o bin/esShader.o bin/esUtil.o bin/esUtil_X11.o
	gcc bin/state.o bin/gameplay.o bin/music.o  bin/note.o bin/input.o bin/gpio.o bin/mat.o bin/vec.o bin/model.o bin/esShader.o bin/esUtil.o bin/esUtil_X11.o -lGLESv2 -lEGL -lX11 -lm -pthread -lgpiod -o rg526

clean:
	rm -f bin/*.o rg526
