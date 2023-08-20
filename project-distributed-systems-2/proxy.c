/*libraries for sockets*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/*standard libraries*/

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>

/*libraries for threads*/
#include <pthread.h>

#include "servidor.h"

#define TRUE 1
#define FALSE 0

#define MAX_CHAIN 255 //tamaño maximo de la cadena
#define MAX_SEND 1024 //tamaño maximo de la cadena a recibir

/*Sockets cliente y servidor*/

int sd, sc;

/*Mantenemos la estructura de peticion para tratarla*/

struct peticion
{
    int op; //valor de la operacion para el switch case 
    int key; //clave
    int key2; //clave 2 para copykey
    char value1[MAX_CHAIN]; //valor 1
    int value2; //valor 2
    double value3; //valor 3

};

/*Mutex y variables para la sincronizacion*/

pthread_mutex_t sync_mutex;
int sync_copied = FALSE;
pthread_cond_t sync_cond; 

int initi = 1;

/*Funcion para tratar las peticiones, llamada a servidor.c que trata los datos*/

void * tratar_peticion(void *p)
{
    
    struct peticion p_local;

    /*Sincronizacion*/

    pthread_mutex_lock(&sync_mutex);
    p_local = (*(struct peticion *) p);

    if (initi == 1 ){

        initi = initialize_mutex();
        initi = 0;

    }
    sync_copied = TRUE;
    pthread_cond_signal(&sync_cond);
    pthread_mutex_unlock(&sync_mutex);

    /*Variables para devolver los datos de get_value()*/

    char res_value1 [MAX_CHAIN];
    int res_value2;
    double res_value3;
    int status;
    char val2[100];
    char val3[100];
    char res[MAX_SEND];

    /*Switch case para tratar las peticiones*/

    switch(p_local.op)
    {
        case 0: status = real_init();
                sprintf(res,"%d", status);
                break;
        case 1: status = real_set_value(p_local.key, p_local.value1, p_local.value2, p_local.value3);
                sprintf(res,"%d", status);
                break;
        case 2: status = real_get_value(p_local.key, res_value1, &res_value2, &res_value3);

                /*Si la clave no existe se devuelve NULL y se pone todo a 0*/

                if (status == -1){
                    strcpy(res_value1, "NULL");
                    res_value2 = 0;
                    res_value3 = 0.0;
                }
                sprintf(res,"%d", status);
                strcat(res, ";");
                strcat(res, res_value1);
                strcat(res, ";");
                sprintf(val2,"%d", res_value2);
                strcat(res,val2);
                strcat(res, ";");
                sprintf(val3, "%lf", res_value3);
                strcat(res, val3);
                break; 
                 
        case 3: status  = real_modify_value(p_local.key, p_local.value1, p_local.value2, p_local.value3);
                sprintf(res,"%d", status);
                break;
        case 4: status = real_delete_key(p_local.key); 
                sprintf(res,"%d", status);
                break;  
        case 5: status = real_exist(p_local.key);
                sprintf(res,"%d", status);
                break;
        case 6: status = real_copy_key(p_local.key, p_local.key2);
                sprintf(res,"%d", status);
                break;
    }

    /*Se abre la cola del cliente y se envia la respuesta*/

    printf("status: %d\n", status);
    printf("Sending message to client\n");

    /*Se envia la respuesta*/

    if (write(sc, (char *)&res, strlen(res)+1) == -1)
    {
        perror("SERVER ERROR: Error al enviar la respuesta\n");
        exit(-1);
    }

    /*Se cierra el socket*/

    close(sc);

    return NULL;

}

int main (int argc, char *argv[])
{

    /*Se comprueba que se ha pasado el puerto*/

    if (argc < 2){

        perror("SERVER ERROR: Puerto del servidor no especificado\n");

    }

    /*Para leer la peticion*/

    char* token;
    int control = 0;

    /*Para tratar la peticion*/

    struct peticion p;

    /*Para los threads*/

    pthread_attr_t t_attr;
    pthread_t thid;

    /*Se inicializa el mutex, condicion y atributo*/

    pthread_mutex_init(&sync_mutex, NULL);
    pthread_cond_init(&sync_cond, NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    /*Socket*/

    char ext [4] = "EXIT";
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int val, err;
    short puerto;
    char cadena[MAX_SEND];

    puerto = atoi(argv[1]);

    /*Se crea el socket*/

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("SERVER ERROR: No se ha podido crear el socket\n");
        return -1;
    }

    val = 1;
    if((err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int))))
    {
        perror("SERVER ERROR: No se ha podido poner operativo el socket\n");
        return -1;
    }

    /*Se obtiene la direccion del servidor*/

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(puerto);

    /*Se asigna la direccion al socket*/

    if (bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("SERVER ERROR: No se ha podido asignar la direccion al socket\n");
        return -1;
    }

    /*Se pone el socket a escuchar*/

    if (listen(sd, SOMAXCONN) < 0)
    {
        perror("SERVER ERROR: No se ha podido poner a escuchar el socket\n");
        return -1;
    }

    size = sizeof(client_addr);

    /*Bucle infinito para aceptar conexiones*/

    while (1){

        if ((sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size)) < 0)
        {
            perror("SERVER ERROR: No se ha podido aceptar la conexion\n");
            return -1;
        }

        /*Se reciben los mensaje*/

        if (read(sc, (char *)&cadena, MAX_SEND) < 0)
        {
            perror("SERVER ERROR: No se ha podido leer del socket\n");
            return -1;
        }

        /*Se procesa el mensaje*/
        
        token = strtok(cadena, ";");

        while( token != NULL ) 
        {
            switch (control)
            {
            case 0:
                if(strcmp(token, ext) == 0){
                    close(sc);
                    close(sd);

                    /*Se destruyen el mutex, condicion y atributo*/

                    pthread_mutex_destroy(&sync_mutex);
                    pthread_cond_destroy(&sync_cond);
                    pthread_attr_destroy(&t_attr);

                    /*Se destruye los mutex para la concurrencia en servidor.c*/

                    destroy_mutex();
                    return -1;
                }
                break;    
            case 1:
                p.op = atoi(token);
                break;

            case 2:
                p.key = atoi(token);
                break;

            case 3:
                p.key2 = atoi(token);
                break;

            case 4:
                strcpy(p.value1, token);
                break;

            case 5:
                p.value2 = atoi(token);
                break;

            case 6:
                p.value3 = atof(token);
                break;
            
            default:
                perror("SERVER ERROR: Error en la peticion\n");
                break;
            }
            
            token = strtok(NULL, ";");
            control += 1;

        }

        /*Se reestablece la variable control para la proxima peticion*/

        control = 0;

        /*Se crea el hilo para tratar la peticion*/

        pthread_create(&thid, &t_attr, tratar_peticion, (void *)&p);

        /*Se espera a que se copie la peticion*/
        
        pthread_mutex_lock(&sync_mutex);
        while(sync_copied == FALSE){
            pthread_cond_wait(&sync_cond, &sync_mutex);
        }
        sync_copied = FALSE;
        pthread_mutex_unlock(&sync_mutex);

    }

    /*Se cierra el socket*/

    close(sd);

    /*Se destruyen el mutex, condicion y atributo*/

    pthread_mutex_destroy(&sync_mutex);
    pthread_cond_destroy(&sync_cond);
    pthread_attr_destroy(&t_attr);

    /*Se destruye los mutex para la concurrencia en servidor.c*/

    destroy_mutex();

    return 0;
    
}