#include "pa2mm.h"
#include "src/estructuras.h"
#include "src/sala.h"
#include "src/objeto.h"
#include "src/interaccion.h"

#include "string.h"
#include <stdbool.h>

void pruebasCrearObjeto()
{
	pa2m_afirmar(objeto_crear_desde_string(NULL) == NULL,
			"No puedo crear un objeto a partir de un string NULL");
	pa2m_afirmar(objeto_crear_desde_string("") == NULL,
			"No puedo crear un objeto a partir de un string vacío");
	pa2m_afirmar(objeto_crear_desde_string("a;b") == NULL,
			"\"a;b\" no es un string objeto válido");
	pa2m_afirmar(objeto_crear_desde_string("a;;b") == NULL,
			"\"a;;b\" no es un string objeto válido");
	pa2m_afirmar(objeto_crear_desde_string(";a;b") == NULL,
			"\";a;b\" no es un string objeto válido");
	pa2m_afirmar(objeto_crear_desde_string("a;b;c") == NULL,
			"\"a;b;c\" no es un string objeto válido");
	pa2m_afirmar(objeto_crear_desde_string("a;;true") == NULL,
			"\"a;;true\" no es un string objeto válido");
	pa2m_afirmar(objeto_crear_desde_string(";a;false") == NULL,
			"\";a;false\" no es un string objeto válido");

	struct objeto *objeto1 = objeto_crear_desde_string("nombre;desc;true");
	pa2m_afirmar(objeto1, "\"nombre;desc;true\" es un string objeto válido");
	pa2m_afirmar(strcmp(objeto1->nombre, "nombre") == 0, "El nombre del objeto es \"nombre\"");
	pa2m_afirmar(strcmp(objeto1->descripcion, "desc") == 0,
			"La descripcion del objeto es \"desc\"");
	pa2m_afirmar(objeto1->es_asible == true, "El objeto es asible");

	struct objeto *objeto2 = objeto_crear_desde_string("A;Be;false\n");
	pa2m_afirmar(objeto2, "\"A;Be;false\\n\" es un string objeto válido");
	pa2m_afirmar(strcmp(objeto2->nombre, "A") == 0, "El nombre del objeto es \"A\"");
	pa2m_afirmar(strcmp(objeto2->descripcion, "Be") == 0,
			"La descripcion del objeto es \"Be\"");
	pa2m_afirmar(objeto2->es_asible == false, "El objeto no es asible");

	free(objeto1);
	free(objeto2);
}

void pruebasCrearInteracciones()
{
	pa2m_afirmar(interaccion_crear_desde_string(NULL) == NULL,
			"No puedo crear una interacción a partir de un string NULL");
	pa2m_afirmar(interaccion_crear_desde_string("") == NULL,
			"No puedo crear una interacción a partir de un string vacío");
	pa2m_afirmar(interaccion_crear_desde_string("a;b;c:d:e") == NULL,
			"\"a;b;c:d:e\" no es un string interacción válido");
	pa2m_afirmar(interaccion_crear_desde_string("a;b;c;d") == NULL,
			"\"a;b;c;d\" no es un string interacción válido");
	pa2m_afirmar(interaccion_crear_desde_string("a;b;c;d:e") == NULL,
			"\"a;b;c;d:e\" no es un string interacción válido");
	pa2m_afirmar(interaccion_crear_desde_string("a;b;_;d:e") == NULL,
			"\"a;b;_;d:e\" no es un string interacción válido");
	pa2m_afirmar(interaccion_crear_desde_string("a;b;_;de:f:g") == NULL,
			"\"a;b;_;de:f:g\" no es un string interacción válido");

	struct interaccion *inter1 = interaccion_crear_desde_string("a;b;c;d:e:f");
	pa2m_afirmar(inter1, "\"a;b;c;d:e:f\" es un string interacción válido");
	pa2m_afirmar(strcmp(inter1->objeto, "a") == 0, "El nombre del objeto es \"a\"");
	pa2m_afirmar(strcmp(inter1->verbo, "b") == 0, "El verbo es \"b\"");
	pa2m_afirmar(strcmp(inter1->objeto_parametro, "c") == 0, "El segundo objeto es \"c\"");
	pa2m_afirmar(inter1->accion.tipo == DESCUBRIR_OBJETO,
			"El tipo de acción es DESCUBRIR_OBJETO");
	pa2m_afirmar(strcmp(inter1->accion.objeto, "e") == 0, "El objeto de la acción es \"e\"");
	pa2m_afirmar(strcmp(inter1->accion.mensaje, "f") == 0, "El mensaje de la acción es \"f\"");

	struct interaccion *inter2 = interaccion_crear_desde_string("OB;VER;_;m:_:MSG");
	pa2m_afirmar(inter1, "\"OB;VER;_;m:_:MSG\" es un string interacción válido");
	pa2m_afirmar(strcmp(inter2->objeto, "OB") == 0, "El nombre del objeto es \"OB\"");
	pa2m_afirmar(strcmp(inter2->verbo, "VER") == 0, "El verbo es \"VER\"");
	pa2m_afirmar(strcmp(inter2->objeto_parametro, "") == 0, "El segundo objeto es vacío");
	pa2m_afirmar(inter2->accion.tipo == MOSTRAR_MENSAJE,
			"El tipo de acción es MOSTRAR_MENSAJE");
	pa2m_afirmar(strcmp(inter2->accion.objeto, "") == 0, "El objeto de la acción es vacío");
	pa2m_afirmar(strcmp(inter2->accion.mensaje, "MSG") == 0,
			"El mensaje de la acción es \"MSG\"");

	free(inter1);
	free(inter2);
}

void pruebas_crear_sala()
{
	pa2m_afirmar(sala_crear_desde_archivos("/ASD/ASD/", "dasD/sa2asdd") == NULL,
			"No puedo crear la sala a partír de archivos inexistentes");

	pa2m_afirmar(sala_crear_desde_archivos("", "chanu/int.csv") == NULL,
			"No puedo crear la sala sin objetos");

	pa2m_afirmar(sala_crear_desde_archivos("chanu/obj.dat", "chanu/vacio.txt") == NULL,
			"No puedo crear la sala sin interacciones");

	sala_t *sala = sala_crear_desde_archivos("chanu/obj.dat", "chanu/int.csv");

	pa2m_afirmar(sala != NULL, "Puedo crear la sala a partir de archivos no vacíos");
	// pa2m_afirmar(sala->cantidad_objetos == 9, "Se leyeron 9 objetos");
	// pa2m_afirmar(sala->cantidad_interacciones == 9, "Se leyeron 9 interacciones");
	
	

	sala_destruir(sala);
}

void pruebas_nombre_objetos()
{
	int cantidad = 0;
	pa2m_afirmar(sala_obtener_nombre_objetos(NULL, &cantidad) == NULL,
			"No puedo obtener los nombres de objetos de una sala NULL");
	pa2m_afirmar(cantidad == -1, "La cantidad es -1 luego de invocar a la función");

	sala_t *sala = sala_crear_desde_archivos("chanu/obj.dat", "chanu/int.csv");

	char **objetos = sala_obtener_nombre_objetos(sala, NULL);
	pa2m_afirmar(objetos != NULL,
			"Puedo pedir el vector de nombres a la sala pasando cantidad NULL");

	char **objetos2 = sala_obtener_nombre_objetos(sala, &cantidad);
	pa2m_afirmar(objetos2 != NULL,
			"Puedo pedir el vector de nombres a la sala pasando cantidad no NULL");
	pa2m_afirmar(cantidad == 9, "La cantidad de elementos del vector coincide con lo esperado");

	// const char *esperados[] = { "habitacion",    "mesa",  "interruptor", "pokebola", "cajon",
	// 			"cajon-abierto", "llave", "anillo",	     "puerta" };

	// int comparaciones_exitosas = 0;

	// for (int i = 0; i < cantidad; i++)
	// 	if (strcmp(objetos2[i], esperados[i]) == 0)
	// 		comparaciones_exitosas++;

	// pa2m_afirmar(comparaciones_exitosas == cantidad,
	// 		"Todos los nombres de objeto son los esperados");

	free(objetos);
	free(objetos2);
	sala_destruir(sala);
}

void pruebas_interacciones()
{
	pa2m_afirmar(sala_es_interaccion_valida(NULL, "hacer", NULL, NULL) == false,
			"No es válido pedir interacciones de una sala NULL");

	sala_t *sala = sala_crear_desde_archivos("chanu/obj.dat", "chanu/int.csv");

	pa2m_afirmar(sala_es_interaccion_valida(sala, NULL, "", "") == false, "No es válida una intearcción con verbo NULL");
	pa2m_afirmar(sala_es_interaccion_valida(sala, "hacer", NULL, "") == false, "No es válida una intearcción con objeto NULL");

	pa2m_afirmar(sala_es_interaccion_valida(sala, "examinar", "habitacion", "") == true, "Puedo examinar la habitación");
	pa2m_afirmar(sala_es_interaccion_valida(sala, "usar", "llave", "cajon") == true, "Puedo usar la llave en el cajón");
	pa2m_afirmar(sala_es_interaccion_valida(sala, "abrir", "pokebola", "") == true, "Puedo abrir la pokebola");
	pa2m_afirmar(sala_es_interaccion_valida(sala, "examinar", "cajon-abierto", "") == true, "Puedo examinar el cajón abierto");
	pa2m_afirmar(sala_es_interaccion_valida(sala, "romper", "todo", "") == false, "No puedo romper todo");
	pa2m_afirmar(sala_es_interaccion_valida(sala, "abrir", "mesa", "") == false, "No puedo abrir la mesa");
	pa2m_afirmar(sala_es_interaccion_valida(sala, "examinar", "techo", "") == false, "No puedo examinar el techo");

	sala_destruir(sala);
}

bool contiene (char **array, char *palabra, int tope){
	for(size_t i = 0; i < tope; i++)
		if(strcmp(array[i],palabra) == 0)
			return true;
	return false;
}


void pruebas_de_funcionamiento_del_juego(){
	sala_t *sala = sala_crear_desde_archivos("ejemplo/objetos.txt","ejemplo/interacciones.txt");
	pa2m_afirmar(sala != NULL, "La sala se creo correctamente");
	int cantidad = 0;
	char ** objetos = sala_obtener_nombre_objetos(sala , &cantidad);

	int cantidad_conocidos;
	char **conocidos = sala_obtener_nombre_objetos_conocidos(sala,&cantidad_conocidos);
	pa2m_afirmar(cantidad_conocidos == 1,"Solo hay un elemento conocido al iniciar el juego");
	pa2m_afirmar(strcmp(conocidos[0],"habitacion") == 0, "Y es el objeto correcto");
	
	free(conocidos);

	int cantidad_inventario = 0;
	char **inventario = sala_obtener_nombre_objetos_poseidos(sala,&cantidad_inventario);
	pa2m_afirmar(cantidad_inventario == 0,"No hay objetos en el inventario al iniciar el juego");
	free(inventario);
	
	pa2m_afirmar(!sala_agarrar_objeto(sala, "llave"), "No se puede agarrar la llave, no la conozco");
	pa2m_afirmar(!sala_agarrar_objeto(sala, "pokebola"), "No se puede agarrar la pokebola, no la conozco");
	pa2m_afirmar(!sala_agarrar_objeto(sala, "puerta"), "No se puede agarrar la puerta, no la conozco");

	pa2m_afirmar(sala_ejecutar_interaccion(sala, "examinar", "habitacion", "", NULL, NULL) == 2, "Al examinar la habitación se ejecutan dos acciones");
	conocidos = sala_obtener_nombre_objetos_conocidos(sala, &cantidad_conocidos);
	pa2m_afirmar(cantidad_conocidos == 3, "Ahora se conocen 3 elementos");
	pa2m_afirmar(contiene(conocidos, "pokebola", cantidad_conocidos) == true && contiene(conocidos, "puerta", cantidad_conocidos) == true,"Se descubrieron los elementos correctos");
	free(conocidos);
	pa2m_afirmar(sala_agarrar_objeto(sala,"pokebola"),"Se puede agarrar la pokebola");
	pa2m_afirmar(!sala_agarrar_objeto(sala, "puerta"), "No se puede agarrar la puerta");
	pa2m_afirmar(!sala_escape_exitoso(sala), "El escape no es exitoso");

	conocidos = sala_obtener_nombre_objetos_conocidos(sala,&cantidad_conocidos);
	inventario = sala_obtener_nombre_objetos_poseidos(sala, &cantidad_inventario);
	pa2m_afirmar(cantidad_conocidos == 2 && cantidad_inventario == 1, "Ahora tengo 2 conocidos y un objeto almacendo");
	free(conocidos);
	free(inventario);
	pa2m_afirmar(sala_ejecutar_interaccion(sala, "abrir", "puerta", "", NULL, NULL) == 0, "No se puede abrir la puerta");
	pa2m_afirmar(sala_ejecutar_interaccion(sala, "abrir", "pokebola", "", NULL, NULL) == 2, "Se ejecutan dos acciones con la pokebola");
	
	conocidos = sala_obtener_nombre_objetos_conocidos(sala, &cantidad_conocidos);
	pa2m_afirmar(contiene(conocidos, "llave",cantidad_conocidos),"Se decubrió una llave");
	free(conocidos);

	pa2m_afirmar(sala_ejecutar_interaccion(sala, "abir", "llave", "puerta", NULL, NULL) == 0, "No se puede abrir la puerta sin tener la llave en el inventario");
	pa2m_afirmar(sala_agarrar_objeto(sala,"llave"), "Puedo agarrar la llave");

	inventario = sala_obtener_nombre_objetos_poseidos(sala, &cantidad_inventario);
	pa2m_afirmar(contiene(inventario,"llave", cantidad_inventario),"La llave está en los objetos poseidos");
	free(inventario);
	pa2m_afirmar(sala_ejecutar_interaccion(sala, "abrir", "llave", "puerta",NULL, NULL) == 1, "Ahora si se puede abir la puerta");

	conocidos = sala_obtener_nombre_objetos_conocidos(sala, &cantidad_conocidos);
	pa2m_afirmar(cantidad_conocidos == 2, "Se conocen dos objetos");
	pa2m_afirmar(contiene(conocidos, "habitacion", cantidad_conocidos) && contiene(conocidos, "puerta-abierta", cantidad_conocidos), "Y son la puerta abierta y la habitación");
	free(conocidos);
	pa2m_afirmar(sala_ejecutar_interaccion(sala, "examinar", "habitacion", "", NULL, NULL) == 0, "No se puede descubir nada nuevo al examinar la habitación");

	conocidos = sala_obtener_nombre_objetos_conocidos(sala, &cantidad_conocidos);
	pa2m_afirmar(cantidad_conocidos == 2, "No se decubrieron más objetos");
	pa2m_afirmar(contiene(conocidos, "habitacion", cantidad_conocidos) && contiene(conocidos, "puerta-abierta", cantidad_conocidos), "Los objetos conocidos son los correctos");
	pa2m_afirmar(sala_agarrar_objeto(sala,"pokebola") == false, "No puedo agarrar la pokebola, ya no existe");

	pa2m_afirmar(!sala_escape_exitoso(sala),"Hay que salir de la habitación para escapar");
	pa2m_afirmar(!sala_agarrar_objeto(sala,"puerta-abierta"), "No puedo agarrar la puerta abierta");
	pa2m_afirmar(sala_ejecutar_interaccion(sala, "salir","puerta-abierta","",NULL,NULL) == 1, "Se puede salir por la puerta abierta");
	pa2m_afirmar(sala_escape_exitoso(sala), "El escape fue exitoso");
	sala_destruir(sala);
	free(objetos);
	free(conocidos);

}

int main()
{
	pa2m_nuevo_grupo("Pruebas de creación de objetos");
	pruebasCrearObjeto();

	pa2m_nuevo_grupo("Pruebas de creación de interacciones");
	pruebasCrearInteracciones();

	pa2m_nuevo_grupo("Pruebas de creación de sala");
	pruebas_crear_sala();

	pa2m_nuevo_grupo("Pruebas del vector de nombres");
	pruebas_nombre_objetos();

	pa2m_nuevo_grupo("Pruebas de interacciones");
	pruebas_interacciones();
	pa2m_nuevo_grupo("Pruebas de funcionamiento del juego");
	pruebas_de_funcionamiento_del_juego();

	return pa2m_mostrar_reporte();
}
