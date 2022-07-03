#include "src/sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINEA 100
#define MAX_PALABRA 20

#define RESET_COLOR "\x1b[0m"
#define ROJO "\x1b[31m"
#define VERDE "\x1b[32m"
#define AMARILLO "\x1b[33m"
#define AZUL "\x1b[34m"
#define BLANCO "\x1b[37m"
#define CYAN "\x1b[36m"
#define MAGENTA "\x1b[35m"


enum comandos {AYUDA, AGARRAR, DESCRIBIR, SALIR};

struct comandos_basicos{
	char *nombre;
	char *ayuda;
};

const struct comandos_basicos basicos[] = {
    {"ayuda", "Escribiendo \"ayuda\", se obtiene un listado de todos los comandos disponibles"},
    {"agarrar", "Escribiendo \"agarrar <objeto>\", se intenta agarrar el objeto si se conoce"},
    {"describir", "Escribiendo \"describir <objeto>\", se describe el objeto si se conoce"},
    {"salir", "Escribiendo \"salir\", se sale del juego"}
    };

void ayuda(char **comandos, size_t cantidad);
void como_jugar(char **comandos, size_t cantidad);
char **obtener_comandos_interacciones(const char *string, size_t *cantidad);
void destruir_comandos(char **comandos, size_t cantidad);
bool iniciar_juego ();
void convertir_a_minuscula(char *string);
void destruir_juego(sala_t *sala, char **comandos, size_t cantidad_comandos);
char **conventir_a_array(char *strin, size_t *cantidad);
void destruir_parametros(char **parametros, size_t cantidad);
void imprimir_objetos(char **objetos_conocidos, char **inventario, size_t conocidos, size_t poseidos);
int procesar_comando_basico(sala_t *sala, char **paramentros, size_t cant_parametros, char **comandos, size_t cantidad_comandos);
int procecesar_comando_sala(sala_t *sala, char **parametros, size_t cant_parametros);





int main(int argc, char *argv[])
{
	if (argc != 3){
		fprintf(stderr, "Para iniciar el juego hay que proporcionar una sala\n");
		fprintf(stderr, "La sala se crea con: [%s] [objetos.txt] [iteracciones.txt]\n", argv[0]);
		return -1;
	}
	sala_t *sala = sala_crear_desde_archivos(argv[1], argv[2]);
	if (sala == NULL) {
		printf("Error al crear la sala de escape\n");
		fprintf(stderr, "La sala se crea con: [%s] [objetos.txt] [iteracciones.txt]\n",argv[0]);
		return -1;
	}
	size_t cantidad_comandos = 0;
	char **comandos = obtener_comandos_interacciones(argv[2], &cantidad_comandos);
	if (!comandos){
		sala_destruir(sala);
		return -1;
	}
	
	printf("\n");
	como_jugar(comandos,cantidad_comandos);
	bool comenzar = iniciar_juego();
	if (!comenzar){
		destruir_juego(sala,comandos,cantidad_comandos);
		return 0;
	}
	printf("Ya se puede comenzar el juego, ¡buena suerte!\n");

	while(!sala_escape_exitoso(sala)){

		int cantidad_conocidos = 0;
		char **objetos_conocidos = sala_obtener_nombre_objetos_conocidos(sala,&cantidad_conocidos);

		int cantidad_inventario = 0;
		char **inventario = sala_obtener_nombre_objetos_poseidos(sala, &cantidad_inventario);
		imprimir_objetos(objetos_conocidos,inventario, (size_t)cantidad_conocidos, (size_t)cantidad_inventario);

		free(objetos_conocidos);
		free(inventario);

		printf("\n>");
		char buff[MAX_LINEA];
		if(!fgets(buff,MAX_LINEA,stdin)){
			destruir_juego(sala, comandos, cantidad_comandos);
			return -1;
		}
		convertir_a_minuscula(buff);
		size_t cantidad_parametros = 0;
		char **parametros = conventir_a_array(buff,&cantidad_parametros);
		if (!parametros){
			fprintf(stderr, ROJO"Ingresar acción\n"RESET_COLOR);
			continue;
		}
		printf("\n");
		int procesado = -1;
		int ejecutadas = 0;
		if (cantidad_parametros < 3){
			procesado = procesar_comando_basico (sala,parametros,cantidad_parametros,comandos,cantidad_comandos);
			if (procesado == SALIR){
				printf("Saliste del juego, " AMARILLO"¡Nos vemos!\n" RESET_COLOR);
			
				return 0;
			}
			if (procesado > 0){
				destruir_parametros(parametros, cantidad_parametros);
				continue;
			}
		}
		
		if (cantidad_parametros >= 2)
			ejecutadas = procecesar_comando_sala(sala,parametros,cantidad_parametros);

		if (procesado == -1 && ejecutadas == 0){
			fprintf(stderr,ROJO"Acción u objeto no inválido\n"RESET_COLOR);
		}

		destruir_parametros(parametros, cantidad_parametros);
	}

	destruir_juego(sala, comandos, cantidad_comandos);
	return 0;
}






 void mostrar_mensaje(const char *mensaje, enum tipo_accion accion, void *aux)
{	
	printf(AMARILLO "%s\n" RESET_COLOR, mensaje);
}

int procecesar_comando_sala(sala_t *sala, char **parametros, size_t cant_parametros)
{
	if (cant_parametros == 2)
		return sala_ejecutar_interaccion(sala,parametros[0],parametros[1],"", mostrar_mensaje,NULL);
	return sala_ejecutar_interaccion(sala, parametros[0], parametros[1], parametros[2], mostrar_mensaje, NULL);

}


void imprimir_objetos (char **objetos_conocidos, char **inventario, size_t conocidos, size_t poseidos)
{
	printf("\n");
	printf(CYAN"Objetos conocidos: %ld\t",conocidos);
	printf("Inventario: %ld\n" RESET_COLOR,poseidos);
	for(size_t i = 0, j = 0; i < conocidos || j < poseidos; i++){
		if (j >= poseidos && i < conocidos)
			printf("%s\n", objetos_conocidos[i]);
		else if (i >=conocidos && j < poseidos)
			printf("\t\t\t%s\n", inventario[j]);
		else {
		printf("%s\t", objetos_conocidos[i]);
		printf("\t\t%s\n",inventario[j]);
		}
		j++;
	}
	printf(CYAN"-----------------------------------------\n"RESET_COLOR);
	
}

int procesar_comando_basico(sala_t *sala, char **parametros, size_t cant_parametros, char **comandos, size_t cantidad_comandos)
{
	if (cant_parametros == 1){
		if (!strcmp(parametros[0], "ayuda")){
			ayuda(comandos, cantidad_comandos);
			return AYUDA;
		}
		else if (!strcmp(parametros[0], "salir")){
			destruir_parametros(parametros, cant_parametros);
			destruir_juego(sala, comandos, cantidad_comandos);
			return SALIR;
		}
	}
	if (cant_parametros == 2){
		if (!strcmp(parametros[0], "agarrar")){
			bool agarro = sala_agarrar_objeto(sala, parametros[1]);
			if (agarro == false)
				return -1;
			return AGARRAR;
		}
		if (!strcmp(parametros[0], "describir")){
			char *descripcion = sala_describir_objeto(sala, parametros[1]);
			if (!descripcion)
				return -1;
			printf("%s\n",descripcion);
			return DESCRIBIR;
		}
	}

	return -1;
}

char** conventir_a_array(char *string, size_t *cantidad){
	char str[3][MAX_PALABRA];
	*cantidad = (size_t)sscanf(string,"%s %s %s",str[0],str[1],str[2]);
	char **parametros = malloc(*cantidad * sizeof(char *));
	if (!parametros)
		return NULL;
	if (cantidad == 0)
		return parametros;
	for(size_t i = 0; i < *cantidad; i++){
		parametros[i] = malloc((strlen(str[i]) + 1) * sizeof(char));
		if (!parametros[i]){
			destruir_parametros(parametros, i);
			return NULL;
		}
		strcpy(parametros[i], str[i]);
	}
	return parametros;
}

void destruir_parametros(char **parametros, size_t cantidad)
{
	if (!parametros)
		return;
	for (size_t i = 0; i < cantidad; i++)
		free(parametros[i]);
	free(parametros);
}
void destruir_juego(sala_t *sala, char **comandos, size_t cantidad_comados)
{
	destruir_comandos(comandos,cantidad_comados);
	sala_destruir(sala);
	return 0;
}

void convertir_a_minuscula(char *string){
	size_t longitud = strlen(string);
	for(size_t i = 0; i < longitud; i++)
		string[i] = (char)tolower(string[i]);
}

bool iniciar_juego (){
	char buff[MAX_LINEA];
	while(fgets(buff, MAX_LINEA,stdin) !=NULL){
		convertir_a_minuscula(buff);
	
		if (!strncmp(buff, "salir", strlen("salir")))
			return false;
		if (strncmp(buff,"comenzar",strlen("comenzar")) != 0){
			fprintf(stdout, "Comando incorrecto, pruebe otra vez\n");
			fprintf(stdout, "O escriba salir para terminar el juego\n");
		}
		if (strncmp(buff, "comenzar", strlen("comenzar")) == 0)
			return true;
	}
	return false;
}


void ayuda(char **comandos, size_t cantidad){
	printf("Comandos Básicos:\n");
	size_t tamanio = sizeof(basicos)/sizeof(basicos[0]);
	for(size_t i = 0; i < tamanio; i++)
		printf(MAGENTA"%s:"RESET_COLOR " %s\n",basicos[i].nombre,basicos[i].ayuda);
	printf("Comandos de esta sala:\n");
	for(size_t i = 0; i < cantidad; i++)
		printf(CYAN"%s\n"RESET_COLOR, comandos[i]);
	printf("Tené en cuenta que los parámetros se leen en el orden" AZUL" <verbo> <objeto 1> <objeto 2>\n"RESET_COLOR);
}

#define CAPACIDAD_INICIAL 5

char **obtener_comandos_interacciones(const char *string, size_t *cantidad){
	if (!string)	
		return NULL;
	FILE *archivo = fopen(string, "r");
	if (!archivo)
		return NULL;
	char buff[MAX_LINEA];
	size_t i = 0;
	char **comandos = calloc(CAPACIDAD_INICIAL,sizeof(char *));
	if (!comandos)
		return NULL;
	size_t capacidad = CAPACIDAD_INICIAL;
	while (fgets(buff, sizeof(buff), archivo) != NULL){
		if (i >= capacidad){
			capacidad += CAPACIDAD_INICIAL; 
			char **aux = realloc(comandos, (i + capacidad)*sizeof(char*));
			if (!aux)
				return NULL;
			comandos = aux;
		}
		char accion[MAX_PALABRA], str1[MAX_PALABRA],str2[MAX_LINEA];
		int leidos = sscanf(buff, "%[^;];%[^;];%[^\n]\n",str1,accion,str2);
		if(leidos < 3)
			return NULL;
		if (i > 0 && !strcmp(accion,comandos[i-1]))
			continue;
		size_t longitud = strlen(accion) + 1;
		comandos[i] = calloc(longitud,sizeof(char));
		if (!comandos[i])
			return NULL;
		strcpy(comandos[i],accion);
		i++;
	}
	if (capacidad< i){
		for (size_t j = i; j < capacidad; j++)
			free(comandos[j]);
	}
	*cantidad = i;
	fclose(archivo);
	return comandos;
}


void como_jugar(char **comandos, size_t cantidad){
	for (size_t i = 0; i < 100; i++)
		printf("-");
	printf("\n\n");
	
	printf(AMARILLO"Bienvenido al mundo pokemon, donde... ¿Eh? ¿Dónde estas?\n");
	printf("Te despertas en una habitación desconocida y no recordas lo que estabas haciendo.\n");
	printf("Miras a tu alrededor y notas una única puerta cerrada, \n");
	printf("la única posibilidad para salir afuera y escapar de esta lugar. ¿Podrás lograrlo?\n" RESET_COLOR);
	printf(MAGENTA"Presiona enter para avanzar\n"RESET_COLOR);
	int n = fgetc(stdin);
	while (n != '\n'){
		n = fgetc(stdin);
	}	
	printf("Para interactuar con la sala contás con los siguientes comandos:\n\n");
	ayuda(comandos, cantidad);
	printf("\n");
	printf("Para inciar el juego escriba " VERDE"comenzar\n"RESET_COLOR);
}

void destruir_comandos(char **comandos, size_t cantidad){
	if (!comandos)
		return;
	for (size_t i = 0; i < cantidad; i++)
		free(comandos[i]);
	free(comandos);
}