#include "hash.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>


#define CAPACIDAD_MINIMA 3
#define MAX_FACTOR_DE_CARGA 0.6
#define SEED 0x92685f5e

typedef struct nodo {
	char *clave;
	void *valor;
	struct nodo *siguiente;
}par_t;

struct hash {
	size_t capacidad;
	size_t almacenados;
	par_t **tabla;
};

static par_t *par_crear(const char *clave, void *valor, par_t *siguiente);

static par_t *encadenar_par(hash_t *hash, par_t *actual, const char *clave, void *elemento,
		    void **anterior, bool *error);


/*
 * Función Hash, dada una clave la transforma en un entero positivo
 */
static size_t djb2(const char *str)
{
	size_t  hash = 5381;
	int c;

	while ((c = *str++) != 0)
		hash = ((hash << 5) + hash) + (size_t)c;

	return hash;
}


hash_t *hash_crear(size_t capacidad)
{
	hash_t *hash = calloc (1, sizeof(hash_t));
	if (!hash) 
		return NULL;
	
	if (capacidad < CAPACIDAD_MINIMA)
		capacidad = CAPACIDAD_MINIMA;

	hash->tabla = calloc(capacidad, sizeof(par_t *));
	if (!hash->tabla){
		free(hash);
		return NULL;
	}
	hash->capacidad = capacidad;
	return hash;
}

/*
 *Recibe un hash y devuelve el mismo hash
 * con la capacidad duplicada
 */
static hash_t *rehash (hash_t *hash)
{
	hash_t *nuevo = hash_crear (hash->capacidad * 2);
	if (!nuevo)
		return NULL;
	for (size_t i = 0; i < hash->capacidad; i++){
		par_t *actual = hash->tabla[i];
		while (actual){
			par_t *siguiente = actual->siguiente;
			size_t posicion = djb2(actual->clave) % nuevo->capacidad;
			
			actual->siguiente = nuevo->tabla[posicion];
			nuevo->tabla[posicion] = actual;
			
			actual = siguiente;
		}
	}
	nuevo->almacenados = hash->almacenados;
	hash_t aux = *hash;
	*hash = *nuevo;
	*nuevo = aux;
	free(nuevo->tabla);
	free(nuevo);

	return hash;
}


hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;
	float factor_de_carga = (((float)hash->almacenados + 1) / (float)hash->capacidad);
	
	if (factor_de_carga >= MAX_FACTOR_DE_CARGA){
		hash_t *aux = rehash(hash);
		if (!aux)
			return NULL;
		hash = aux;
	}
	
	size_t posicion = djb2(clave) % hash->capacidad;

	bool hubo_error = false;
	hash->tabla[posicion] = encadenar_par(hash, hash->tabla[posicion], clave,elemento, anterior, &hubo_error);
	if (hubo_error == true)
		return NULL;

	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;
	size_t posicion = djb2(clave) % hash->capacidad;
	par_t *anterior = NULL;
	par_t *actual = hash->tabla[posicion];

	while(actual && strcmp(actual->clave, clave)){		
		anterior = actual;
		actual = actual->siguiente;
	}
	if (!actual)
		return NULL;

	void *valor = actual->valor;
	if (!anterior)
		hash->tabla[posicion] = actual->siguiente;
	
	else
		anterior->siguiente = actual->siguiente;

	hash->almacenados--;
	free(actual->clave);
	free(actual);
	return valor;
}


void *hash_obtener(hash_t * hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;
	size_t posicion = djb2(clave) % hash->capacidad;
	par_t *actual = hash->tabla[posicion];
	while (actual != NULL){
		if (strcmp(actual->clave, clave) == 0)
			return actual->valor;
		actual = actual->siguiente;
	}
	return NULL;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return false;
	size_t posicion = djb2(clave) % hash->capacidad;
	par_t *actual = hash->tabla[posicion];

	while (actual){
		if (!strcmp(actual->clave, clave))
			return true;
		par_t *siguiente = actual->siguiente;
		actual = siguiente;
	}
	
	return false;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->almacenados;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	for (size_t i = 0; i < hash->capacidad;i++){
		par_t *actual = hash->tabla[i];
		while (actual){
			if(destructor)
				destructor(actual->valor);
			par_t *siguiente = actual->siguiente;
			free(actual->clave);
			free(actual);
			actual = siguiente;
		}
	}
	free(hash->tabla);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	if (!hash || !f)
		return 0;
	size_t recorridos = 0;
	bool seguir_recorriendo = true;
	for (size_t i = 0; i < hash->capacidad && seguir_recorriendo; i++){
		par_t *actual = hash->tabla[i];
		while (actual && seguir_recorriendo){
			seguir_recorriendo = f(actual->clave, actual->valor, aux);
			actual = actual->siguiente;
			recorridos++;
		}
	}
	return recorridos;
}

/*
 *Recibe una clave y su valor, siendo la clave un string distinto de NULL.
 * Recibe un puntero al proximo nodo par
 *  
 * Devuelve un nodo par con los parametros clave-valor recibidos y un puntero 
 * al siguiente elemento.
 * O devuelve NULL en caso de error.
 */
static par_t *par_crear(const char *clave, void *valor, par_t *siguiente){
	par_t *par = malloc(sizeof(par_t));
	if (!par)
		return NULL;

	size_t len = strlen(clave) + 1;
	par->clave = malloc(len * sizeof(char));
	if(!par->clave){
		free(par);
		return NULL;
	}

	strcpy(par->clave, clave);
	par->siguiente = siguiente;
	par->valor = valor;

	return par;
}

/*
 * Recibe un nodo inicial en el cual comienza el recorrido para insertar un elemento
 * Recibe un string clave distinto de NULL y su valor asociado
 * Recibe un puntero a bool distinto de NULL.
 * 
 * Devuelve el nodo inicial con el elemento recibido actualizado o encadenado al final.
 * En caso de error le asigna false al booleano.
 */
static par_t *encadenar_par(hash_t *hash, par_t *actual, const char *clave, void *elemento,
		     void **anterior, bool *error)
{
	if (!actual){
		par_t *nuevo = par_crear(clave, elemento, NULL);
		if (!nuevo){
			*error = true;
			return NULL;
		}
		actual = nuevo;
		if (anterior)
			*anterior = NULL;
		hash->almacenados++;
		return actual;
	}
	if (strcmp(actual->clave, clave) == 0){
		if (anterior != NULL)
			*anterior = actual->valor;
		actual->valor = elemento;
		return actual;
	}
	actual->siguiente = encadenar_par(hash, actual->siguiente, clave, elemento, anterior, error);
	return actual;
}


