/*standard libraries*/

#include<string.h>
#include<stdlib.h>
#include<stdio.h>

/*libraries for queues*/

#include<fcntl.h>
#include<sys/stat.h>
#include<mqueue.h>

/*libraries for threads*/
#include <pthread.h>

#include "servidor.h"

#define TRUE 1
#define FALSE 0

#define MAX_CHAIN 255 //tamaño maximo de la cadena
#define MAX_QUEUE 1024 //tamaño maximo de la cola

struct peticion
{
    int op; //valor de la operacion para el switch case 
    int key; //clave
    int key2; //clave 2 para copykey
    char value1[MAX_CHAIN]; //valor 1
    int value2; //valor 2
    double value3; //valor 3
    char q_name[MAX_QUEUE]; //nombre de la cola
};

struct respuesta
{
    char value1[MAX_CHAIN]; //valor de la respuesta
    int value2; //valor 2
    double value3; //valor 3
    int status; //estado de la respuesta
};

mqd_t q_server;
mqd_t q_client;

/*Atributos para la cola de servidor*/

struct mq_attr q_attrs = {
    .mq_flags = 0,
    .mq_maxmsg = 10,
    .mq_msgsize = sizeof(struct peticion),
    .mq_curmsgs = 0
};

/*Atributos para la cola de cliente*/

struct mq_attr q_attrc = {
    .mq_flags = 0,
    .mq_maxmsg = 10,
    .mq_msgsize = sizeof(struct respuesta),
    .mq_curmsgs = 0
};

/*Mutex y variables para la sincronizacion*/

pthread_mutex_t sync_mutex;
int sync_copied = FALSE;
pthread_cond_t sync_cond; 

int initi = 1;

/*Función para tratar las peticiones, llamada a servidor.c que trata los datos*/

void * tratar_peticion(void *p)
{

    struct peticion p_local;
    struct respuesta r;
    
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

    /*Switch case para tratar las peticiones*/

    switch(p_local.op)
    {
        case 0: r.status = real_init();
                break;
        case 1: r.status = real_set_value(p_local.key, p_local.value1, p_local.value2, p_local.value3);
                break;
        case 2: r.status = real_get_value(p_local.key, res_value1, &res_value2, &res_value3);

                /*Si la clave no existe se devuelve NULL y se pone todo a 0*/

                if (r.status == -1){
                    strcpy(res_value1, "NULL");
                    res_value2 = 0;
                    res_value3 = 0;
                }
                strcpy(r.value1, res_value1);
                r.value2 = res_value2;
                r.value3 = res_value3;
                break; 
                 
        case 3: r.status  = real_modify_value(p_local.key, p_local.value1, p_local.value2, p_local.value3);
                break;
        case 4: r.status = real_delete_key(p_local.key); 
                break;  
        case 5: r.status = real_exist(p_local.key);
                break;
        case 6: r.status = real_copy_key(p_local.key, p_local.key2);
                break;
    }

    /*Se abre la cola del cliente y se envia la respuesta*/

    printf("status: %d\n",r.status);
    q_client = mq_open(p_local.q_name, O_WRONLY, &q_attrc);
    if (q_client == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }
    mq_send(q_client, (char *)&r, sizeof(r), 0);

    /*Se cierra la cola del cliente*/

    mq_close(q_client);

    /*Se cierra el hilo*/

    pthread_exit(NULL);

}

int main (void)
{

    struct peticion p;
    pthread_attr_t t_attr;
    pthread_t thid;

    /*Se abre la cola del servidor*/

    q_server = mq_open("/server", O_CREAT|O_RDONLY, 0700, &q_attrs);
    if (q_server == -1)
    {
        perror("Error al abrir la cola del servidor");
        exit(-1);
    }

    /*Se inicializa el mutex, condicion y atributo*/

    pthread_mutex_init(&sync_mutex, NULL);
    pthread_cond_init(&sync_cond, NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    /*Bucle infinito para recibir peticiones*/

    while (1){

        /*Se recibe la peticion*/

        mq_receive(q_server, (char *)&p, sizeof(p), 0);

        /*Se crea el hilo para tratar la peticion*/

        pthread_create(&thid, &t_attr, tratar_peticion, (void *)&p);

        /*Se espera a que se copie la peticion*/
        
        pthread_mutex_lock(&sync_mutex);
        while(sync_copied == FALSE){
            pthread_cond_wait(&sync_cond, &sync_mutex);
        }
        sync_copied = FALSE;
        pthread_mutex_unlock(&sync_mutex);

        /*Se espera a que termine el hilo*/

        pthread_join(thid, NULL);
    }

    /*Se cierra la cola del servidor*/

    mq_close(q_server);
    mq_unlink("/server");

    /*Se destruyen el mutex, condicion y atributo*/

    pthread_mutex_destroy(&sync_mutex);
    pthread_cond_destroy(&sync_cond);
    pthread_attr_destroy(&t_attr);

    /*Se destruye los mutex para la concurrencia en servidor.c*/

    destroy_mutex();
    
}
