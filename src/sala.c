#include "estructuras.h"
#include "sala.h"
#include "objeto.h"
#include "interaccion.h"
#include "lista.h"
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINEA 200


struct jugador{
	hash_t *objetos_conocidos;
	hash_t *inventario;
}jugador_t;

struct sala
{
	struct jugador *jugador;
	hash_t *objetos;
	size_t cantidad_objetos;
	lista_t *interacciones;
	size_t cantidad_interacciones;
};

static sala_t *sala_crear_vacia();
static bool leer_objetos(sala_t *sala, FILE *archivo);
static bool leer_interaccion(sala_t *sala, FILE *archivo);


sala_t *sala_crear_desde_archivos(const char *objetos, const char *interacciones)
{
	FILE *archivo_objetos = fopen(objetos, "r");
	if (!archivo_objetos)
		return NULL;

	FILE *archivo_interacciones = fopen(interacciones, "r");
	if (!archivo_interacciones){
		fclose(archivo_objetos);
		return NULL;
	}

	sala_t *sala = sala_crear_vacia();
	if (sala == NULL){
		fclose(archivo_interacciones);
		fclose(archivo_objetos);
		return NULL;
	}
	if (!leer_objetos(sala, archivo_objetos) || 
		!leer_interaccion(sala, archivo_interacciones)){

		sala_destruir(sala);
		fclose(archivo_objetos);
		fclose(archivo_interacciones);
		return NULL;
	}

	fclose(archivo_interacciones);
	fclose(archivo_objetos);
	if (sala->cantidad_interacciones == 0 || sala->cantidad_interacciones == 0){
		sala_destruir(sala);
		return NULL;
	}
	return sala;
}

/*
 *-----------------------------------Objetos-----------------------------------
 */

struct vector_objetos {
	char **objetos;
	size_t tope;
};

static bool guardar_objetos_en_vector(const char *clave, void *valor, void *aux);

static char **_obtener_nombre_objetos(hash_t *objetos, int *cantidad, size_t tamanio)
{
	struct vector_objetos tmp;
	tmp.objetos = calloc((size_t)tamanio, sizeof(char *));
	if (tmp.objetos == NULL){
		if (cantidad != NULL)
			*cantidad = -1;
		return NULL;
	}
	tmp.tope = 0;
	size_t aux = hash_con_cada_clave(objetos, guardar_objetos_en_vector, &tmp);
	if (aux != tamanio){
		if (cantidad != NULL)
			*cantidad = -1;
		free(tmp.objetos);
		return NULL;
	}
	if (cantidad)
		*cantidad = (int)tamanio;
	return tmp.objetos;
}



char **sala_obtener_nombre_objetos(sala_t *sala, int *cantidad)
{
	if (!sala && cantidad){
		*cantidad = -1;
		return NULL;
	}
	else if (!sala && !cantidad)
		return NULL;
	
	return _obtener_nombre_objetos(sala->objetos,cantidad, sala->cantidad_objetos);
}

char **sala_obtener_nombre_objetos_conocidos(sala_t *sala, int *cantidad)
{
	if (!sala && cantidad){
		*cantidad = -1;
		return NULL;
	}
	else if (!sala && !cantidad)
		return NULL;
	
	size_t cantidad_de_objetos = hash_cantidad (sala->jugador->objetos_conocidos);
	return _obtener_nombre_objetos(sala->jugador->objetos_conocidos, cantidad, cantidad_de_objetos);
}

char **sala_obtener_nombre_objetos_poseidos(sala_t *sala, int *cantidad){
	if (!sala && cantidad){
		*cantidad = -1;
		return NULL;
	}
	else if (!sala && !cantidad)
		return NULL;

	size_t cantidad_de_objetos = hash_cantidad(sala->jugador->inventario);
	return _obtener_nombre_objetos(sala->jugador->inventario, cantidad, cantidad_de_objetos);
}

bool sala_agarrar_objeto(sala_t *sala, const char *nombre_objeto)
{
	if (!sala || !nombre_objeto)
		return false;
	struct objeto *objeto = hash_quitar(sala->jugador->objetos_conocidos, nombre_objeto);
	if (objeto->es_asible == false){
		sala->jugador->objetos_conocidos = hash_insertar(sala->jugador->objetos_conocidos, nombre_objeto,objeto, NULL);
		return false;
	}

	sala->jugador->inventario = hash_insertar(sala->jugador->inventario,nombre_objeto, objeto,NULL);
	if (!sala->jugador->inventario)
		return false;
	return true;
}

char *sala_describir_objeto(sala_t *sala, const char *nombre_objeto)
{
	if (!sala || !nombre_objeto)
		return NULL;
	struct objeto *objeto = hash_obtener(sala->jugador->objetos_conocidos, nombre_objeto);
	if (!objeto)
		objeto = hash_obtener(sala->jugador->inventario, nombre_objeto);
	if (!objeto)
		return NULL;
	return objeto->descripcion;
}

/*
 *-----------------------------------Interacciones------------------------------------
 */

struct interaccion_valida
{
	struct interaccion inter;
	bool valida;
};


static bool es_interaccion_valida (void *elemento, void *extra);

bool sala_es_interaccion_valida(sala_t *sala, const char *verbo, const char *objeto1,
				const char *objeto2)
{
	if (!sala || !verbo || !objeto1 || !objeto2)
		return false;

	struct interaccion_valida temp;
	strcpy(temp.inter.verbo,verbo);
	strcpy(temp.inter.objeto, objeto1);
	strcpy(temp.inter.objeto_parametro, objeto2);
	temp.valida = false;
	lista_con_cada_elemento(sala->interacciones, es_interaccion_valida,&temp);
	return temp.valida;
}

int sala_ejecutar_interaccion(sala_t *sala, const char *verbo,
			      const char *objeto1, const char *objeto2,
			      void (*mostrar_mensaje)(const char *mensaje,
						enum tipo_accion accion,
						      void *aux),
			      void *aux);

bool sala_es_interaccion_valida(sala_t *sala, const char *verbo,
				const char *objeto1, const char *objeto2);

bool sala_escape_exitoso(sala_t *sala);

void sala_destruir(sala_t *sala)
{
	if (!sala)
		return;
	hash_destruir_todo(sala->objetos,free);
	lista_destruir_todo(sala->interacciones,free);
	free(sala);
}

/*
 *Devuelve una sala con sus elementos inicializados
 * En el caso de los objetos, se crea un hash vacío
 * Y en el caso de las iteracciones, una lista vacía;
 * Ademas crea un jugador con 0 elementos conocidos
 * Y con el inventario vacio
*/
static sala_t *sala_crear_vacia()
{
	sala_t *sala = calloc(1,sizeof(sala_t));
	if (sala == NULL)
		return NULL;
	sala->interacciones = lista_crear();
	sala->objetos = hash_crear(0);
	sala->jugador = calloc(1,(sizeof(jugador_t)));
	if (!sala->interacciones || !sala->objetos || !sala->jugador)
		return NULL;
	sala->jugador->inventario = hash_crear(0);
	sala->jugador->objetos_conocidos = hash_crear(0);
	if (!sala->jugador->inventario || !sala->jugador->objetos_conocidos)
		return NULL;
	return sala;
}

/*
 * Lee los objetos de un archivo, y los guarda en una sala existente
 * Ademas guarda el primer objeto leido en los conocidos del jugador
 * Devuelve true si pudo agregar todos los objetos a la sala
 * O devuelve false en caso de haber fallado 
*/
static bool leer_objetos(sala_t *sala, FILE *archivo)
{
	char buff[MAX_LINEA];
	while (fgets(buff, sizeof(buff), archivo) != NULL)
	{
		struct objeto *objeto = objeto_crear_desde_string(buff);
		if (!objeto)
			return false;	
		if (sala->cantidad_objetos == 0){
			sala->jugador->objetos_conocidos = hash_insertar(sala->jugador->objetos_conocidos,objeto->nombre, objeto, NULL);
			if (!sala->jugador->objetos_conocidos)
				return false;
		}	
		sala->objetos = hash_insertar(sala->objetos,objeto->nombre,objeto,NULL);
		if (!sala->objetos)
			return false;
		sala->cantidad_objetos++;
	}
	return true;
}


/*
 * Recibe una sala no nula, y un archivo con interacciones
 * Devuelve true si logro guardar todas las interacciones en la sala
 * O devuelve false si fallo
 */
static bool leer_interaccion(sala_t *sala, FILE *archivo)
{
	char buff[MAX_LINEA];
	while (fgets(buff, sizeof(buff), archivo) != NULL)
	{
		struct interaccion *interaccion = interaccion_crear_desde_string(buff);
		if (interaccion == NULL)
			return false;
		sala->interacciones = lista_insertar(sala->interacciones,interaccion);
		if (!sala->interacciones)
			return false;
		sala->cantidad_interacciones++;
	}
	return true;
}


/*
 * Función para recorrer el hash de objetos
 * Devuelve false si ya no quedan elementos
 * O devuelve true si debe seguir ejecutandose
 */
static bool guardar_objetos_en_vector(const char *clave, void *valor, void *aux)
{
	if (!clave || !valor || !aux)
		return false;
	struct vector_objetos *objetos = aux;
	objetos->objetos[objetos->tope] = (char *)clave;
	objetos->tope++;
	return true;
}

/*
 * Función para recorrer la lista de interacciones
 * Devuelve false si no quedan elementos o si la interaccion es valida
 * En caso contrario devuelve true para seguir recorriendo la lista.
 */
static bool es_interaccion_valida(void *elemento, void *extra)
{
	if (!elemento || !extra)
		return false;
	struct interaccion *interaccion1 = elemento;
	struct interaccion_valida *interaccion2 = extra;
	if (!strcmp(interaccion1->objeto, interaccion2->inter.objeto) &&
	    !strcmp(interaccion1->objeto_parametro, interaccion2->inter.objeto_parametro) &&
	    !strcmp(interaccion1->verbo, interaccion2->inter.verbo))
	{
		interaccion2->valida = true;
		return false;
	}
	return true;
}

