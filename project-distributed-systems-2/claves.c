/*libraries for sockets*/

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*standard libraries*/

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include"claves.h"
#include<unistd.h>
#include<errno.h>

int sd;

#define MAX_CHAIN 255 //tamaño maximo de la cadena
#define MAX_SEND 1024 //tamaño maximo de la cadena de envio

int init()
{

    /*Para el uso de sockets*/

    int sd;
    short puerto;
    struct sockaddr_in server_addr;
    struct hostent *hp; 
    char cadena[MAX_SEND];
    char res[MAX_SEND];

    /*Se hace el mensaje a enviar*/

    strcpy(cadena, "init;0;0;0;NULL;0;0");

    char *var1;
    char *var2;

    var1 = getenv("PORT_TUPLAS");
    var2 = getenv("IP_TUPLAS");

    if (var1 == NULL || var2 == NULL){

        perror("ERROR: IP o PORT no encontrado\n");

    }

    puerto = atoi(var1);

    /*Se crea el socket*/

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: No se ha podido abrir el socket\n");
        exit(1);
    }

    /*Se obtiene la direccion del servidor*/

    bzero((char *)&server_addr, sizeof(server_addr));
    printf("gethost\n");
    hp = gethostbyname (var2);

   memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   server_addr.sin_family  = AF_INET;
   server_addr.sin_port    = htons(puerto);

    /*Se conecta con el servidor*/

    printf("Connecting to server\n");
    
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("ERROR: No se ha podido conectar al servidor\n");
		return -1;
	}

    printf("Connected\n");

    /*Se envia el mensaje*/

    if (write(sd, (char *)&cadena, strlen(cadena)+1) < 0)
    {
        perror("ERROR: No se ha podido escribir en el socket\n");
        exit(1);
    }

    /*Se recibe la respuesta*/

    if (read(sd, (char *)&res, MAX_SEND) < 0)   
    {
        perror("ERROR: No se ha podido leer del socket\n");
        exit(1);
    }

    int status = atoi(res);

    /*Se cierra el socket*/

    close(sd);

    return status;

}

int set_value(int key, char *value1, int value2, double value3)
{

    /*Para el uso de sockets*/

    int sd;
    short puerto;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char cadena[MAX_SEND];
    char res[MAX_SEND];

    /*Para hacer la cadena del mensaje*/

    char k[100], val2[100], val3[100];
    sprintf(k,"%d", key);       
    sprintf(val2,"%d", value2);
    sprintf(val3,"%lf",value3);

    char *var1;
    char *var2;

    var1 = getenv("PORT_TUPLAS");
    var2 = getenv("IP_TUPLAS");

    if (var1 == NULL || var2 == NULL){

        perror("ERROR: IP o PORT no encontrado\n");

    }
    
    puerto = atoi(var1);

    /*Se hace el mensaje a enviar*/
    
    strcpy(cadena, "set_value");
    strcat(cadena, ";1;");
    strcat(cadena, k);
    strcat(cadena, ";0;");
    strcat(cadena, value1);
    strcat(cadena, ";");
    strcat(cadena, val2);
    strcat(cadena, ";");
    strcat(cadena, val3);

    /*Se crea el socket*/

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: No se ha podido abrir el socket\n");
        exit(1);
    }

    /*Se obtiene la direccion del servidor*/

    bzero((char *)&server_addr, sizeof(server_addr));
    printf("gethost\n");
    hp = gethostbyname (var2);

   memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   server_addr.sin_family  = AF_INET;
   server_addr.sin_port    = htons(puerto);

    /*Se conecta con el servidor*/

    printf("Connecting to server\n");
    
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("ERROR: No se ha podido conectar al servidor\n");
		return -1;
	}

    printf("Connected\n");

    /*Se envia el mensaje*/

    if (write(sd, (char *)&cadena, strlen(cadena)+1) < 0)
    {
        perror("ERROR: No se ha podido escribir en el socket\n");
        exit(1);
    }

    /*Se recibe la respuesta*/

    if (read(sd, (char *)&res, MAX_SEND) < 0)   
    {
        perror("ERROR: No se ha podido leer del socket\n");
        exit(1);
    }

    int status = atoi(res);

    /*Se cierra el socket*/

    close(sd);

    return status;

}
int get_value(int key, char *value1, int *value2, double *value3)
{

    /*Para leer la respuesta*/

    char* token;
    int control = 0;

    /*Para el uso de sockets*/

    int sd;
    short puerto;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char cadena[MAX_CHAIN];
    char res[MAX_SEND];

    /*Para hacer la cadena del mensaje*/

    char k[100];
    sprintf(k,"%d", key);

    char *var1;
    char *var2;

    var1 = getenv("PORT_TUPLAS");
    var2 = getenv("IP_TUPLAS");

    if (var1 == NULL || var2 == NULL){

        perror("ERROR: IP o PORT no encontrado\n");

    }
    
    puerto = atoi(var1);

    /*Se hace el mensaje a enviar*/

    strcpy(cadena, "get_value");
    strcat(cadena, ";2;");
    strcat(cadena, k);
    strcat(cadena, ";0;NULL;0;0.0");

    /*Se crea el socket*/

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: No se ha podido abrir el socket\n");
        exit(1);
    }

    /*Se obtiene la direccion del servidor*/

    bzero((char *)&server_addr, sizeof(server_addr));
    printf("gethost\n");
    hp = gethostbyname (var2);

   memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   server_addr.sin_family  = AF_INET;
   server_addr.sin_port    = htons(puerto);

    /*Se conecta con el servidor*/

    printf("Connecting to server\n");
    
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("ERROR: No se ha podido conectar al servidor\n");
		return -1;
	}

    printf("Connected\n");

    /*Se envia el mensaje*/

    if (write(sd, (char *)&cadena, strlen(cadena)+1) < 0)
    {
        perror("ERROR: No se ha podido escribir en el socket\n");
        exit(1);
    }

    /*Se recibe la respuesta*/

    if (read(sd, (char *)&res, MAX_SEND) < 0)   
    {
        perror("ERROR: No se ha podido leer del socket\n");
        exit(1);
    }

    /*Se lee la respuesta*/

    token = strtok(res, ";");

    int status = 2; 

    while( token != NULL ) {
      switch (control)
      {
        case 0:
            status = atoi(token);
            break;
        case 1:
            strcpy(value1, token);
            break;

        case 2:
            *value2 = atoi(token);
            break;

        case 3:
            *value3 = atof(token);
            break;
      
        default:
            perror("ERROR: Respuesta del servidor no valida\n");
            break;

      }
        control+= 1;
        token = strtok(NULL, ";");
   }

    /*Se cierra el socket*/

    close(sd);

    return status;
    
}
int modify_value(int key, char *value1, int value2, double value3)
{

    /*Para el uso de sockets*/

    int sd;
    short puerto;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char cadena[MAX_CHAIN];
    char res[MAX_SEND];

    /*Para hacer la cadena del mensaje*/

    char k[100], val2[100], val3[100];
    sprintf(k,"%d", key);       
    sprintf(val2,"%d", value2);
    sprintf(val3,"%lf",value3);

    char *var1;
    char *var2;

    var1 = getenv("PORT_TUPLAS");
    var2 = getenv("IP_TUPLAS");

    if (var1 == NULL || var2 == NULL){

        perror("ERROR: IP o PORT no encontrado\n");

    }
    
    puerto = atoi(var1);

    /*Se hace el mensaje a enviar*/

    strcpy(cadena, "modify_value");
    strcat(cadena, ";3;");
    strcat(cadena, k);
    strcat(cadena, ";0;");
    strcat(cadena, value1);
    strcat(cadena, ";");
    strcat(cadena, val2);
    strcat(cadena, ";");
    strcat(cadena, val3);

    /*Se crea el socket*/

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: No se ha podido abrir el socket\n");
        exit(1);
    }

    /*Se obtiene la direccion del servidor*/

    bzero((char *)&server_addr, sizeof(server_addr));
    printf("gethost\n");
    hp = gethostbyname (var2);

   memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   server_addr.sin_family  = AF_INET;
   server_addr.sin_port    = htons(puerto);

    /*Se conecta con el servidor*/

    printf("Connecting to server\n");
    
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("ERROR: No se ha podido conectar al servidor\n");
		return -1;
	}

    printf("Connected\n");

    /*Se envia el mensaje*/

    if (write(sd, (char *)&cadena, strlen(cadena)+1) < 0)
    {
        perror("ERROR: No se ha podido escribir en el socket\n");
        exit(1);
    }

    /*Se recibe la respuesta*/

    if (read(sd, (char *)&res, MAX_SEND) < 0)   
    {
        perror("ERROR: No se ha podido leer del socket\n");
        exit(1);
    }

    int status = atoi(res);

    /*Se cierra el socket*/

    close(sd);

    return status;

} 
int delete_key(int key)
{

    /*Para el uso de sockets*/

    int sd;
    short puerto;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char cadena[MAX_CHAIN];
    char res[MAX_SEND];

    /*Para hacer la cadena del mensaje*/

    char k[100];
    sprintf(k, "%d", key);

    char *var1;
    char *var2;

    var1 = getenv("PORT_TUPLAS");
    var2 = getenv("IP_TUPLAS");

    if (var1 == NULL || var2 == NULL){

        perror("ERROR: IP o PORT no encontrado\n");

    }
    
    puerto = atoi(var1);

    /*Se hace el mensaje a enviar*/

    strcpy(cadena, "delete_key");
    strcat(cadena, ";4;");
    strcat(cadena, k);
    strcat(cadena, ";0;NULL;0;0.0");

    /*Se crea el socket*/

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: No se ha podido abrir el socket\n");
        exit(1);
    }

    /*Se obtiene la direccion del servidor*/

    bzero((char *)&server_addr, sizeof(server_addr));
    printf("gethost\n");
    hp = gethostbyname (var2);

   memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   server_addr.sin_family  = AF_INET;
   server_addr.sin_port    = htons(puerto);

    /*Se conecta con el servidor*/

    printf("Connecting to server\n");
    
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}

    printf("Connected\n");

    /*Se envia el mensaje*/

    if (write(sd, (char *)&cadena, strlen(cadena)+1) < 0)
    {
        perror("ERROR: No se ha podido escribir en el socket\n");
        exit(1);
    }

    /*Se recibe la respuesta*/

    if (read(sd, (char *)&res, MAX_SEND) < 0)   
    {
        perror("ERROR: No se ha podido leer del socket\n");
        exit(1);
    }

    int status = atoi(res);

    /*Se cierra el socket*/

    close(sd);

    return status;
    
}
int exist(int key)
{

    /*Para el uso de sockets*/

    int sd;
    short puerto;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char cadena[MAX_CHAIN];
    char res[MAX_SEND];

    /*Para hacer la cadena del mensaje*/

    char k[100];
    sprintf(k,"%d",key);

    char *var1;
    char *var2;

    var1 = getenv("PORT_TUPLAS");
    var2 = getenv("IP_TUPLAS");

    if (var1 == NULL || var2 == NULL){

        perror("ERROR: IP o PORT no encontrado\n");

    }
    
    puerto = atoi(var1);

    /*Se hace el mensaje a enviar*/

    strcpy(cadena, "exist");
    strcat(cadena, ";5;");
    strcat(cadena, k);
    strcat(cadena, ";0;NULL;0;0.0");

    /*Se crea el socket*/

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: No se ha podido abrir el socket\n");
        exit(1);
    }

    /*Se obtiene la direccion del servidor*/

    bzero((char *)&server_addr, sizeof(server_addr));
    printf("gethost\n");
    hp = gethostbyname (var2);

   memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   server_addr.sin_family  = AF_INET;
   server_addr.sin_port    = htons(puerto);

    /*Se conecta con el servidor*/

    printf("Connecting to server\n");
    
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("ERROR: No se ha podido conectar al servidor\n");
		return -1;
	}

    printf("Connected\n");

    /*Se envia el mensaje*/

    if (write(sd, (char *)&cadena, strlen(cadena)+1) < 0)
    {
        perror("ERROR: No se ha podido escribir en el socket\n");
        exit(1);
    }

    /*Se recibe la respuesta*/

    if (read(sd, (char *)&res, MAX_SEND) < 0)   
    {
        perror("ERROR: No se ha podido leer del socket\n");
        exit(1);
    }

    int status = atoi(res);

    /*Se cierra el socket*/

    close(sd);

    return status;

}
int copy_key(int key1, int key2)
{

    /*Para el uso de sockets*/

    int sd;
    short puerto;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char cadena[MAX_CHAIN];
    char res[MAX_SEND];

    /*Para hacer la cadena del mensaje*/

    char k1[100], k2[100];
    sprintf(k1, "%d", key1);
    sprintf(k2, "%d", key2);

    char *var1;
    char *var2;

    var1 = getenv("PORT_TUPLAS");
    var2 = getenv("IP_TUPLAS");

    if (var1 == NULL || var2 == NULL){

        perror("ERROR: IP o PORT no encontrado\n");

    }
    
    puerto = atoi(var1);

    /*Se hace el mensaje a enviar*/

    strcpy(cadena, "copy_key");
    strcat(cadena, ";6;");
    strcat(cadena, k1);
    strcat(cadena, ";");
    strcat(cadena, k2);
    strcat(cadena, ";NULL;0;0.0");

    /*Se crea el socket*/

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: No se ha podido abrir el socket\n");
        exit(1);
    }

    /*Se obtiene la direccion del servidor*/

    bzero((char *)&server_addr, sizeof(server_addr));
    printf("gethost\n");
    hp = gethostbyname (var2);

   memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   server_addr.sin_family  = AF_INET;
   server_addr.sin_port    = htons(puerto);

    /*Se conecta con el servidor*/

    printf("Connecting\n");
    
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("ERROR: No se ha podido conectar al servidor\n");
		return -1;
	}

    printf("Connected\n");

    /*Se envia el mensaje*/

    if (write(sd, (char *)&cadena, strlen(cadena)+1) < 0)
    {
        perror("ERROR: No se ha podido escribir en el socket\n");
        exit(1);
    }

    /*Se recibe la respuesta*/

    if (read(sd, (char *)&res, MAX_SEND) < 0)   
    {
        perror("ERROR: No se ha podido leer del socket\n");
        exit(1);
    }

    int status = atoi(res);

    /*Se cierra el socket*/

    close(sd);

    return status;
    
}
