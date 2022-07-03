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

struct sala
{
	hash_t *objetos;
	lista_t *interacciones;
	hash_t *objetos_conocidos;
	hash_t *inventario;
	bool exito;
};

static sala_t *sala_crear_vacia();
static bool leer_objetos(sala_t *sala, const char *path_objetos);
static bool leer_interaccion(sala_t *sala,const char *path_interacciones);

sala_t *sala_crear_desde_archivos(const char *objetos, const char *interacciones)
{
	sala_t *sala = sala_crear_vacia();
	if (sala == NULL){
		return NULL;
	}
	if (!leer_objetos(sala, objetos) || 
		!leer_interaccion(sala, interacciones)){
		sala_destruir(sala);
		return NULL;
	}
	
	if (hash_cantidad(sala->objetos) == 0 || lista_tamanio(sala->interacciones) == 0){
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
static char **_obtener_nombre_objetos(hash_t *objetos, int *cantidad, size_t tamanio);

char **sala_obtener_nombre_objetos(sala_t *sala, int *cantidad)
{
	if (!sala && cantidad){
		*cantidad = -1;
		return NULL;
	}
	else if (!sala && !cantidad)
		return NULL;

	return _obtener_nombre_objetos(sala->objetos, cantidad, hash_cantidad(sala->objetos));
}

char **sala_obtener_nombre_objetos_conocidos(sala_t *sala, int *cantidad)
{
	if (!sala && cantidad){
		*cantidad = -1;
		return NULL;
	}
	else if (!sala && !cantidad)
		return NULL;
	
	size_t cantidad_de_objetos = hash_cantidad (sala->objetos_conocidos);
	return _obtener_nombre_objetos(sala->objetos_conocidos, cantidad, cantidad_de_objetos);
}

char **sala_obtener_nombre_objetos_poseidos(sala_t *sala, int *cantidad){
	if (!sala && cantidad){
		*cantidad = -1;
		return NULL;
	}
	else if (!sala && !cantidad)
		return NULL;

	size_t cantidad_de_objetos = hash_cantidad(sala->inventario);
	return _obtener_nombre_objetos(sala->inventario, cantidad, cantidad_de_objetos);
}

bool sala_agarrar_objeto(sala_t *sala, const char *nombre_objeto)
{
	if (!sala || !nombre_objeto)
		return false;
	struct objeto *objeto = hash_obtener(sala->objetos_conocidos, nombre_objeto);

	if (!objeto || objeto->es_asible == false)
		return false;

	hash_quitar(sala->objetos_conocidos, nombre_objeto);
	sala->inventario = hash_insertar(sala->inventario,nombre_objeto, objeto,NULL);
	if (!sala->inventario)
		return false;
	return true;
}

char *sala_describir_objeto(sala_t *sala, const char *nombre_objeto)
{
	if (!sala || !nombre_objeto)
		return NULL;
	struct objeto *objeto = hash_obtener(sala->objetos_conocidos, nombre_objeto);
	if (!objeto)
		objeto = hash_obtener(sala->inventario, nombre_objeto);
	if (!objeto)
		return NULL;
	return objeto->descripcion;
}

/*
 *-----------------------------------Interacciones------------------------------------
 */

struct ejecutar{
	sala_t *sala;
	struct interaccion inter;
	void (*f)(const char *mensaje, enum tipo_accion accion, void *aux);
	void *aux;
	size_t ejecutadas;
};

static int ejecutar_accion(sala_t *sala, struct interaccion *interaccion, 
			void (*f)(const char *,enum tipo_accion, void *),void *aux);

static bool ejecutar_interaccion(void *elemento, void *extra);

int sala_ejecutar_interaccion(sala_t *sala, const char *verbo,
			      const char *objeto1, const char *objeto2,
			      void (*mostrar_mensaje)(const char *mensaje,
				enum tipo_accion accion, void *aux), void *aux){
	if(!sala || !verbo || !objeto1)
		return 0;
	if (hash_contiene(sala->objetos_conocidos, objeto1) == false &&
	    hash_contiene(sala->inventario, objeto1) == false)
		return 0;
	
	struct ejecutar tmp;
	tmp.sala = sala;
	strcpy(tmp.inter.verbo, verbo);
	strcpy(tmp.inter.objeto, objeto1);
	strcpy(tmp.inter.objeto_parametro, objeto2);
	
	tmp.f = mostrar_mensaje;
	tmp.aux = aux;
	tmp.ejecutadas = 0;
	lista_con_cada_elemento(sala->interacciones,ejecutar_interaccion,&tmp);
	return (int)tmp.ejecutadas;
}

struct interaccion_valida
{
	struct interaccion inter;
	bool valida;
};

static bool es_interaccion_valida (void *elemento, void *extra);

bool sala_es_interaccion_valida(sala_t *sala, const char *verbo, const char *objeto1,
				const char *objeto2)
{
	if (!sala || !verbo || !objeto1)
		return false;

	struct interaccion_valida tmp;
	strcpy(tmp.inter.verbo,verbo);
	strcpy(tmp.inter.objeto, objeto1);
	strcpy(tmp.inter.objeto_parametro, objeto2);
	tmp.valida = false;
	lista_con_cada_elemento(sala->interacciones, es_interaccion_valida,&tmp);
	return tmp.valida;
}


bool sala_escape_exitoso(sala_t *sala)
{
	if (!sala)
		return false;
	return sala->exito;
}

void sala_destruir(sala_t *sala)
{
	if (!sala)
		return;
	hash_destruir_todo(sala->objetos,free);
	lista_destruir_todo(sala->interacciones,free);
	hash_destruir(sala->objetos_conocidos);
	hash_destruir(sala->inventario);
	free(sala);
}

/*
 * ---------------------------Funciones Privadas---------------------------
 */

/*
 *Devuelve una sala con sus elementos inicializados
 * En el caso de los objetos, se crea un hash vacío
 * Y en el caso de las iteracciones, una lista vacía;
 * Ademas crea un jugador con 0 elementos conocidos
 * Y con el inventario vacio
*/
static sala_t *sala_crear_vacia()
{
	sala_t *sala = calloc(1, sizeof(sala_t));
	if (sala == NULL)
		return NULL;
	sala->interacciones = lista_crear();
	sala->objetos = hash_crear(10);
	sala->inventario = hash_crear(10);
	sala->objetos_conocidos = hash_crear(10);

	if (!sala->interacciones || !sala->objetos ||!sala->inventario || 
	    !sala->objetos_conocidos){
		sala_destruir(sala);
		return NULL;
	}

	sala->exito = false;
	return sala;
}

/*
 * Lee los objetos de un archivo, y los guarda en una sala existente
 * Ademas guarda el primer objeto leido en los conocidos del jugador
 * Devuelve true si pudo agregar todos los objetos a la sala
 * O devuelve false en caso de haber fallado 
*/
static bool leer_objetos(sala_t *sala, const char *path_objetos)
{
	FILE *archivo = fopen(path_objetos, "r");
	if (!archivo)
		return NULL;

	char buff[MAX_LINEA];
	while (fgets(buff, sizeof(buff), archivo) != NULL){
		struct objeto *objeto = objeto_crear_desde_string(buff);
		if (!objeto)
			return false;
		if (hash_cantidad(sala->objetos) == 0){
			sala->objetos_conocidos = hash_insertar(sala->objetos_conocidos,objeto->nombre, objeto, NULL);
			if (!sala->objetos_conocidos)
				return false;
		}
		sala->objetos = hash_insertar(sala->objetos,objeto->nombre,objeto,NULL);
		if (!sala->objetos)
			return false;
	}
	fclose(archivo);
	return true;
}


/*
 * Recibe una sala no nula, y un archivo con interacciones
 * Devuelve true si logro guardar todas las interacciones en la sala
 * O devuelve false si fallo
 */
static bool leer_interaccion(sala_t *sala, const char *path_interacciones)
{
	FILE *archivo = fopen(path_interacciones, "r");
	if (!archivo)
		return NULL;
	char buff[MAX_LINEA];
	while (fgets(buff, sizeof(buff), archivo) != NULL){
		struct interaccion *interaccion = interaccion_crear_desde_string(buff);
		if (interaccion == NULL)
			return false;
		sala->interacciones = lista_insertar(sala->interacciones,interaccion);
		if (!sala->interacciones)
			return false;
	}
	fclose(archivo);
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
 *Recibe un hash, y la cantidad de elementos que tiene, y los guarda
 * en un vector dinamico
 * En caso se fallar devuelve NULL y le asigna a  cantidid (si es posible) un -1
 */
static char **_obtener_nombre_objetos(hash_t *objetos, int *cantidad, size_t tamanio)
{

	struct vector_objetos tmp;
	tmp.objetos = calloc(tamanio, sizeof(char *));
	if (!tmp.objetos){
		if (cantidad != NULL)
			*cantidad = -1;
		return NULL;
	}
	if (tamanio == 0){
		if (cantidad)
			*cantidad = (int)tamanio;
		return tmp.objetos;
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
	    !strcmp(interaccion1->verbo, interaccion2->inter.verbo)){
		interaccion2->valida = true;
		return false;
	}
	return true;
}

int accion_descubrir_objeto(sala_t *sala, struct interaccion *interaccion);
int accion_reemplazar_objeto(sala_t *sala, struct interaccion *interaccion);
int accion_eliminar_objeto(sala_t *sala, struct interaccion *interaccion);
int accion_escapar(sala_t *sala);
int accion_mostrar_mensaje(struct interaccion *interaccion, void (*f)(const char *, enum tipo_accion, void *), void *aux);

/*
 * Ejecuta una accion segun la accion de la interaccion dada
 * Llama a una función f, la cual recibe el mensaje de la accion, el tipo
 * de la accion y puntero auxiliar dado por el usuario
 * Devuelve 0 si logra ejecutar la accíon o -1 si no se ejecuto nada.
 */
static int ejecutar_accion(sala_t *sala, struct interaccion *interaccion, void (*f)(const char *, enum tipo_accion, void *), void *aux)
{
	enum tipo_accion accion = interaccion->accion.tipo;
	int se_ejecuto = -1;
	if (accion == DESCUBRIR_OBJETO)
		se_ejecuto = accion_descubrir_objeto(sala, interaccion);

	if (accion == REEMPLAZAR_OBJETO)
		se_ejecuto = accion_reemplazar_objeto(sala, interaccion);

	if (accion == ELIMINAR_OBJETO)
		se_ejecuto = accion_eliminar_objeto(sala, interaccion);

	if (accion == ESCAPAR){
		se_ejecuto = accion_escapar(sala);
	}
	if (accion == MOSTRAR_MENSAJE)
		se_ejecuto = accion_mostrar_mensaje(interaccion, f, aux);

	else if (se_ejecuto != -1 && accion != MOSTRAR_MENSAJE)
		accion_mostrar_mensaje(interaccion,f,aux);

	return se_ejecuto;
}

int accion_descubrir_objeto(sala_t *sala, struct interaccion *interaccion)
{
	if (hash_contiene(sala->objetos_conocidos, interaccion->accion.objeto) == true ||
	    hash_contiene(sala->inventario, interaccion->accion.objeto) == true ||
	    hash_contiene(sala->objetos, interaccion->accion.objeto) == false)
		return -1;

	struct objeto *objeto_interaccion = hash_obtener(sala->objetos, interaccion->objeto);

	if (objeto_interaccion->es_asible == true && hash_contiene(sala->inventario, objeto_interaccion->nombre) == false)
		return -1;

	struct objeto *objeto = hash_obtener(sala->objetos, interaccion->accion.objeto);
	sala->objetos_conocidos = hash_insertar(sala->objetos_conocidos, objeto->nombre, objeto, NULL);

	return 0;
}

int accion_reemplazar_objeto(sala_t *sala, struct interaccion *interaccion)
{
	if (hash_contiene(sala->inventario, interaccion->objeto) == false)
		return -1;

	struct objeto *objeto = hash_obtener(sala->objetos, interaccion->accion.objeto);
	sala->objetos_conocidos = hash_insertar(sala->objetos_conocidos, objeto->nombre, objeto, NULL);
	hash_quitar(sala->objetos_conocidos, interaccion->objeto_parametro);
	void *objeto_quitado = hash_quitar(sala->objetos, interaccion->objeto_parametro);
	free(objeto_quitado);
	return 0;
}

int accion_eliminar_objeto(sala_t *sala, struct interaccion *interaccion)
{
	hash_quitar(sala->objetos_conocidos, interaccion->accion.objeto);
	hash_quitar(sala->inventario, interaccion->accion.objeto);
	void *objeto = hash_quitar(sala->objetos, interaccion->accion.objeto);
	free(objeto);
	return 0;
}

int accion_escapar(sala_t *sala)
{
	sala->exito = true;
	return 0;
}

int accion_mostrar_mensaje(struct interaccion *interaccion, void (*f)(const char *, enum tipo_accion, void *), void *aux)
{
	if (f == NULL)
		return -1;
	f(interaccion->accion.mensaje, interaccion->accion.tipo, aux);
	return 0;
}

/*
 * Función utilizada para recorrer la lista de interacciones
 * Recibe un puntero extra el cual es una estructura con la sala,
 * la interacción a ejecutar, la cantidad de interacciones ejecutadas,
 * una función f dada por el usuario y un puntero auxiliar para pasarle a la función.
 * Si el objeto y el verbo de interacción a ejecutar coincide con alguno en la lista de interacciones
 * Lo ejecuta y sigue recorriendo la lista 
 */
static bool ejecutar_interaccion(void *elemento, void *extra)
{
	if (!elemento)
		return false;
	struct interaccion *inter = elemento;
	struct ejecutar *ejecutar = extra;
	if (!strcmp(inter->objeto, ejecutar->inter.objeto) &&
	    !strcmp(inter->verbo, ejecutar->inter.verbo) && 
	    !strcmp(inter->objeto_parametro, ejecutar->inter.objeto_parametro)){
		int ejecutada = ejecutar_accion(ejecutar->sala, inter, ejecutar->f, ejecutar->aux);
		if (ejecutada != -1)
			ejecutar->ejecutadas++;
	}
	return true;
}
