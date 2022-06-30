#include "objeto.h"
#include "estructuras.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct objeto *objeto_crear_desde_string(const char *string)
{
	if (string == NULL)
		return NULL;

	struct objeto temp;
	char es_asible[MAX_NOMBRE];

	int elementos_leidos = sscanf(string, "%[^;];%[^;];%[^\n]\n", temp.nombre, temp.descripcion, es_asible);
	if (elementos_leidos != 3)
		return NULL;

	if (!strcmp(es_asible, "true"))
		temp.es_asible = true;
	else if (!strcmp(es_asible, "false"))
		temp.es_asible = false;
	else
		return NULL;

	struct objeto *objeto = calloc(1,sizeof(struct objeto));
	if (objeto == NULL)
		return NULL;

	*objeto = temp;

	return objeto;
}
