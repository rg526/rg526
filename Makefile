all: rg526

rg526.o: rg526.c
	gcc rg526.c -c -o rg526.o -Wall -Ies

mat.o: mat.c
	gcc mat.c -c -o mat.o -Wall -Ies

vec.o: vec.c
	gcc vec.c -c -o vec.o -Wall -Ies

model.o: model.c
	gcc model.c -c -o model.o -Wall -Ies

es/esShader.o: es/esShader.c
	gcc es/esShader.c -c -o es/esShader.o 

es/esUtil.o: es/esUtil.c
	gcc es/esUtil.c -c -o es/esUtil.o

es/esUtil_X11.o: es/esUtil_X11.c
	gcc es/esUtil_X11.c -c -o es/esUtil_X11.o

rg526: rg526.o mat.o vec.o model.o es/esShader.o es/esUtil.o es/esUtil_X11.o
	gcc rg526.o mat.o vec.o model.o es/esShader.o es/esUtil.o es/esUtil_X11.o -lGLESv2 -lEGL -lX11 -lm -o rg526

clean:
	rm -f *.o es/*.o rg526
