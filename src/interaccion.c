#include "estructuras.h"
#include "interaccion.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char letra_tipo_accion[] = {[ACCION_INVALIDA] = 0, [DESCUBRIR_OBJETO] = 'd', 
					[REEMPLAZAR_OBJETO] = 'r', [ELIMINAR_OBJETO] = 'e', 
					[MOSTRAR_MENSAJE] = 'm', [ESCAPAR]='g'};

static enum tipo_accion letra_a_accion(char accion);

static bool accion_desde_string(struct accion *accion, const char *string);

struct interaccion *interaccion_crear_desde_string(const char *string)
{
	if (string == NULL)
		return NULL;

	struct interaccion temp;
	char string_accion[MAX_TEXTO];

	int elementos_leidos = sscanf(string, "%[^;];%[^;];%[^;];%[^\n]\n",
					temp.objeto, temp.verbo, temp.objeto_parametro, string_accion);
	if (elementos_leidos != 4)
		return NULL;
	if (!accion_desde_string(&temp.accion, string_accion))
		return NULL;

	if (!strcmp(temp.objeto_parametro, "_"))
		strcpy(temp.objeto_parametro, "");

	struct interaccion *interaccion = malloc(sizeof(struct interaccion));
	if (interaccion == NULL)
		return NULL;
	*interaccion = temp;

	return interaccion;
}

static enum tipo_accion letra_a_accion(char accion)
{
	size_t cantidad = sizeof(letra_tipo_accion);
	for (size_t i = 0; i <= cantidad; i++)
	{
		if (accion == letra_tipo_accion[i])
			return i;
	}
	return ACCION_INVALIDA;
}

static bool accion_desde_string(struct accion *accion, const char *string)
{
	if (string == NULL)
		return false;
	struct accion temp;
	char tipo_accion;
	int elementos_leidos = sscanf(string, "%c:%[^:]:%[^\n]\n", &tipo_accion, temp.objeto, temp.mensaje);
	if (elementos_leidos != 3)
		return false;

	if (!strcmp(temp.objeto, "_"))
		strcpy(temp.objeto, "");

	temp.tipo = letra_a_accion(tipo_accion);

	*accion = temp;
	return true;
}
