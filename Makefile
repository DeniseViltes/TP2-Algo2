PROGRAM=escape_pokemon
CC=gcc
CFLAGS= -Wall -std=c99 -Wconversion -Wtype-limits -Werror -pedantic -g -O1
OBJETOS=ejemplo/objetos.txt
INTERACCIONES=ejemplo/interacciones.txt

all: $(PROGRAM)

$(PROGRAM): escape_pokemon.o sala.o interaccion.o objeto.o
	$(CC) $(CFLAGS) escape_pokemon.o sala.o interaccion.o objeto.o -o $(PROGRAM)


pruebas.o: pruebas.c pa2mm.h sala.o interaccion.o objeto.o
	$(CC) $(CFLAGS) -c pruebas.c

escape_pokemon.o: escape_pokemon.c src/sala.h sala.o 
	$(CC) $(CFLAGS) -c escape_pokemon.c

sala.o: src/sala.c src/sala.h src/estructuras.h src/objeto.h src/interaccion.h
	$(CC) $(CFLAGS) -c src/sala.c

interaccion.o: src/interaccion.c src/interaccion.h src/estructuras.h
	$(CC) $(CFLAGS) -c src/interaccion.c

objeto.o: src/objeto.c src/objeto.h src/estructuras.h
	$(CC) $(CFLAGS) -c src/objeto.c

valgrind: $(PROGRAM)
	valgrind ${VFLAGS} ./$(PROGRAM) ${OBJETOS} ${INTERACCIONES}

clean:
	rm -vf *.o $(PROGRAM)