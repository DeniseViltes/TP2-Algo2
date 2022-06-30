#include "lista.h"
#include <stddef.h>
#include <stdlib.h>

struct nodo
{
	void *elemento;
	struct nodo *siguiente;
};

struct lista
{
	nodo_t *nodo_inicio;
	nodo_t *nodo_fin;
	size_t cantidad;
};

struct lista_iterador
{
	nodo_t *corriente;
	lista_t *lista;
};

static nodo_t *lista_nodo_crear(void *elemento, nodo_t *siguiente);
static void recorrer_lista (nodo_t **actual, size_t posicion);

lista_t *lista_crear()
{
	lista_t *lista = calloc (1,sizeof (lista_t)); 

	return lista;
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (!lista)
		return NULL;

	nodo_t *nuevo = lista_nodo_crear(elemento, NULL);
	if (!nuevo)
		return NULL;

	if (!lista->cantidad ){
		lista->nodo_inicio = nuevo;
		lista->nodo_fin = nuevo;
		lista->cantidad ++;
		return lista; 
	}
	lista->nodo_fin->siguiente = nuevo;

	lista->nodo_fin = nuevo;
	lista->cantidad++;

	return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				size_t posicion)
{
	if (!lista)
		return NULL;

	if (posicion >= lista->cantidad || !lista->cantidad)
		return lista_insertar(lista, elemento);

	if (posicion == 0)
	{
		nodo_t *nuevo = lista_nodo_crear(elemento, lista->nodo_inicio);
		if (!nuevo)
			return NULL;
		lista->nodo_inicio = nuevo;
		lista->cantidad++;
		return lista;
	}
	nodo_t *actual = lista->nodo_inicio;

	recorrer_lista(&actual, posicion-1);
	nodo_t *nuevo = lista_nodo_crear(elemento, actual->siguiente);
	if (!nuevo)
		return NULL;

	actual->siguiente = nuevo;
	lista->cantidad++;
	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (!lista || !lista->cantidad )
		return NULL;
	
	nodo_t *actual = lista->nodo_inicio;
	void *dato = lista_ultimo(lista);

	if (lista->cantidad == 1){
		free (actual);
		lista->nodo_inicio = NULL;
		lista->nodo_fin = NULL;
		lista->cantidad--;
		return dato;
	}

	recorrer_lista(&actual,(lista->cantidad)-2);
	free(actual->siguiente);

	lista->nodo_fin = actual;
	actual->siguiente = NULL;
	lista->cantidad--;

	return dato;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (!lista)
		return NULL;

	if (posicion >= lista->cantidad)
		return lista_quitar(lista);

	nodo_t *actual = lista->nodo_inicio;
	if (posicion == 0){
		lista->nodo_inicio = actual->siguiente;
		void *dato = actual->elemento;
		free(actual);
		lista->cantidad--;
		return dato;
	}
	
	recorrer_lista(&actual, posicion-1);
	
	nodo_t *temp = actual->siguiente->siguiente;
	void *dato = actual->siguiente->elemento;
	free(actual->siguiente);
	actual->siguiente = temp;
	lista->cantidad--;
	return dato;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista  || (posicion >= lista->cantidad))
		return NULL;
	if (posicion == (lista->cantidad -1))
		return lista_ultimo(lista);
	nodo_t *actual = lista->nodo_inicio;
	recorrer_lista(&actual, posicion);
	return actual->elemento;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (!lista || !comparador)
		return NULL;
	nodo_t *actual = lista->nodo_inicio;
	while (actual){
		if (!comparador (actual->elemento,contexto))
			return actual->elemento;
		actual =actual->siguiente;
	}
	return NULL;
}

void *lista_primero(lista_t *lista)
{
	if (!lista || !lista->nodo_inicio )
		return NULL;

	return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if (!lista || lista_vacia(lista) )
		return NULL;

	return lista->nodo_fin->elemento;
}

bool lista_vacia(lista_t *lista)
{
	if (!lista)
		return true;
	return !lista->cantidad;
}

size_t lista_tamanio(lista_t *lista)
{
	if (!lista)	
		return 0;
	return lista->cantidad;
}

void lista_destruir(lista_t *lista)
{
	lista_destruir_todo(lista,NULL);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista)
		return;
	nodo_t *actual = lista->nodo_inicio;
	while (actual != NULL){
		nodo_t *siguiente = actual->siguiente;
		if (funcion != NULL)
			funcion(actual->elemento);
		free(actual);
		actual=siguiente;
	}
	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista)
		return NULL;
	lista_iterador_t *iterador= malloc(sizeof(lista_iterador_t));
	if (!iterador)
		return NULL;
	iterador->corriente = lista->nodo_inicio;
	iterador->lista = lista;
	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->corriente)
		return false;
	return true;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (!lista_iterador_tiene_siguiente(iterador))
		return false;

	iterador->corriente = iterador->corriente->siguiente;

	return iterador->corriente !=NULL;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->corriente)
		return NULL;
	return iterador->corriente->elemento;
} 

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (!lista || !funcion)
		return 0;
	size_t i = 0;
	nodo_t *actual = lista->nodo_inicio;
	bool seguir_recorriendo = true;
	while (actual && seguir_recorriendo){
		seguir_recorriendo = funcion(actual->elemento,contexto);
		actual=actual->siguiente;
		i++;
	}
	return i;
}

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

static nodo_t *lista_nodo_crear(void *elemento, nodo_t *siguiente)
{
	nodo_t *nodo = malloc (sizeof(nodo_t));
	if (!nodo)	
		return NULL;
	nodo->elemento = elemento;
	nodo->siguiente = siguiente;
	return nodo;
}

static void recorrer_lista(nodo_t **actual, size_t posicion){
	size_t i = 0; 
	while (i < posicion){
		*actual= (*actual)->siguiente;
		i++;
	}
}