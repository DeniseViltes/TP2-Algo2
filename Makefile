PROGRAM = escape_pokemon
CC = gcc
CFLAGS = -Wall -std=c99 -Wconversion -Wtype-limits -Werror -pedantic -g -O1
OBJETOS = ejemplo/objetos.txt
INTERACCIONES = ejemplo/interacciones.txt

all: clean $(PROGRAM)

$(PROGRAM): escape_pokemon.c src/*
	$(CC) $(CFLAGS) src/*.c escape_pokemon.c -o $(PROGRAM)

pruebas: pruebas.c pa2mm.h src/*
	$(CC) $(CFLAGS) src/*.c pruebas.c -o pruebas

valgrind: $(PROGRAM)
	valgrind ${VFLAGS} ./$(PROGRAM) ${OBJETOS} ${INTERACCIONES}

valgrind-pruebas: pruebas
	valgrind --leak-check=full ./pruebas

clean:
	rm -vf /*.o $(PROGRAM) pruebas