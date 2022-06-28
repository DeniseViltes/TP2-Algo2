PROGRAM = escape_pokemon
CC = gcc
CFLAGS = -Wall -std=c99 -Wconversion -Wtype-limits -Werror -pedantic -g -O1
OBJETOS = ejemplo/objetos.txt
INTERACCIONES = ejemplo/interacciones.txt
LIMPIAR = escape_pokemon.o sala.o interaccion.o objeto.o 

all: clean $(PROGRAM)

$(PROGRAM): escape_pokemon.c src/*
	$(CC) $(CFLAGS) src/*.c src/*.o escape_pokemon.c -o $(PROGRAM)

pruebas: pruebas.c pa2mm.h src/*
	$(CC) $(CFLAGS) src/*.c src/*.o pruebas.c -o pruebas

valgrind: $(PROGRAM)
	valgrind ${VFLAGS} ./$(PROGRAM) ${OBJETOS} ${INTERACCIONES}

valgrind-pruebas: pruebas
	valgrind $(VALGRIND_FLAGS) ./pruebas

clean:
	rm -vf $(LIMPIAR) $(PROGRAM) pruebas