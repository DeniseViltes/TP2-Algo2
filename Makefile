PROGRAM = escape_pokemon
CC = gcc
CFLAGS = -Wall -std=c99 -Wconversion -Wtype-limits -Werror -pedantic -g -O2
OBJETOS = ejemplo/objetos.txt
INTERACCIONES = ejemplo/interacciones.txt

all: clean jugar

$(PROGRAM): escape_pokemon.c src/*
	$(CC) $(CFLAGS) src/*.c escape_pokemon.c -o $(PROGRAM)

pruebas: pruebas.c pa2mm.h src/*
	$(CC) $(CFLAGS) src/*.c pruebas.c -o pruebas

valgrind: $(PROGRAM)
	valgrind --leak-check=full ./$(PROGRAM) ${OBJETOS} ${INTERACCIONES}

valgrind-pruebas: pruebas
	valgrind --leak-check=full ./pruebas

jugar: $(PROGRAM)
	./$(PROGRAM) ${OBJETOS} ${INTERACCIONES}

clean:
	rm -vf /*.o $(PROGRAM) pruebas