all: rg526

bin/gameplay.o: gameplay.c
	gcc gameplay.c -c -o bin/gameplay.o -Wall -Ies

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

rg526: bin/gameplay.o bin/mat.o bin/vec.o bin/model.o bin/esShader.o bin/esUtil.o bin/esUtil_X11.o
	gcc bin/gameplay.o bin/mat.o bin/vec.o bin/model.o bin/esShader.o bin/esUtil.o bin/esUtil_X11.o -lGLESv2 -lEGL -lX11 -lm -o rg526

clean:
	rm -f bin/*.o rg526
