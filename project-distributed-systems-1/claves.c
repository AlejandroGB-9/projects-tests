/*libraries for queues*/

#include<fcntl.h>
#include<sys/stat.h>
#include<mqueue.h>

/*standard libraries*/

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include"claves.h"
#include<unistd.h> 

mqd_t q_server;
mqd_t q_client;

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

int init()
{

    struct peticion p;
    struct respuesta r;
    char q_name[MAX_QUEUE];

    /*Apertura de colas de servidor y cliente con sus atribuitos y permisos*/

    sprintf(q_name, "%s%d", "/client_", getpid());      /*Se crea el nombre de la cola del cliente con su pid*/
    q_server = mq_open("/server", O_WRONLY, &q_attrs);
    if (q_server == -1)
    {
        perror("Error al abrir la cola del servidor");
        exit(-1);
    }
    q_client = mq_open(q_name, O_CREAT | O_RDONLY,0644,&q_attrc);
    if (q_client == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }

    /*Se crea la petición con los datos*/

    strcpy(p.q_name, q_name);
    p.op = 0;

    /*Se envia la petición y espera a la respuesta*/

    mq_send (q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    /*Se cierran las colas y se elimina la cola del cliente*/

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(q_name);

    /*Resultado de la operación*/

    return r.status;

}

int set_value(int key, char *value1, int value2, double value3)
{

    struct peticion p;
    struct respuesta r;
    char q_name[MAX_QUEUE];

    /*Apertura de colas de servidor y cliente con sus atribuitos y permisos*/

    sprintf(q_name, "%s%d", "/client_", getpid());    /*Se crea el nombre de la cola del cliente con su pid*/
    q_server = mq_open("/server", O_WRONLY, &q_attrs);
    if (q_server == -1)
    {
        perror("Error al abrir la cola del servidor");
        exit(-1);
    }
    q_client = mq_open(q_name, O_CREAT | O_RDONLY, 0644,&q_attrc);
    if (q_client == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }

    /*Se crea la petición con los datos*/

    strcpy(p.q_name, q_name);
    p.op = 1;
    p.key = key;
    strcpy(p.value1, value1);
    p.value2 = value2;
    p.value3 = value3;

    /*Se envia la petición y espera a la respuesta*/

    mq_send (q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    /*Se cierran las colas y se elimina la cola del cliente*/

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(q_name);

    /*Resultado de la operación*/

    return r.status;
}
int get_value(int key, char *value1, int *value2, double *value3)
{
    
    struct peticion p;
    struct respuesta r;
    char q_name[MAX_QUEUE];

    /*Apertura de colas de servidor y cliente con sus atribuitos y permisos*/

    sprintf(q_name,"%s%d","/client_",getpid());     /*Se crea el nombre de la cola del cliente con su pid*/
    q_server=mq_open("/server", O_WRONLY, &q_attrs);
     if (q_server == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }
    q_client = mq_open(q_name, O_CREAT | O_RDONLY, 0644,&q_attrc);
    if (q_client == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }

    /*Se crea la petición con los datos*/

    strcpy(p.q_name,q_name);
    p.op = 2;
    p.key = key;

    /*Se envia la petición y espera a la respuesta*/

    mq_send (q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    /*Se cierran las colas y se elimina la cola del cliente*/

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(q_name);

    /*Se copian los valores de la respuesta*/

    strcpy(value1, r.value1); 
    *value2 = r.value2;
    *value3 = r.value3;

    /*Resultado de la operación*/

    return r.status;
    
}
int modify_value(int key, char *value1, int value2, double value3)
{

    struct peticion p;
    struct respuesta r;
    char q_name[MAX_QUEUE];

    /*Apertura de colas de servidor y cliente con sus atribuitos y permisos*/

    sprintf(q_name, "%s%d", "/client_", getpid());      /*Se crea el nombre de la cola del cliente con su pid*/
    q_server = mq_open("/server",  O_WRONLY, &q_attrs);
    if (q_server == -1)
    {
        perror("Error al abrir la cola del servidor");
        exit(-1);
    }
    q_client = mq_open(q_name, O_CREAT | O_RDONLY, 0644,&q_attrc);
    if (q_client == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }

    /*Se crea la petición con los datos*/

    strcpy(p.q_name, q_name);
    p.op = 3;
    p.key = key;
    strcpy(p.value1, value1);
    p.value2 = value2;
    p.value3 = value3;

    /*Se envia la petición y espera a la respuesta*/

    mq_send (q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    /*Se cierran las colas y se elimina la cola del cliente*/

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(q_name);

    /*Resultado de la operación*/

    return r.status;
} 
int delete_key(int key)
{

    struct peticion p;
    struct respuesta r;
    char q_name[MAX_QUEUE];

    /*Apertura de colas de servidor y cliente con sus atribuitos y permisos*/

    sprintf(q_name, "%s%d", "/client_", getpid());      /*Se crea el nombre de la cola del cliente con su pid*/
    q_server = mq_open("/server", O_WRONLY, &q_attrs);
    if (q_server == -1)
    {
        perror("Error al abrir la cola del servidor");
        exit(-1);
    }
    q_client = mq_open(q_name, O_CREAT | O_RDONLY, 0644,&q_attrc);
    if (q_client == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }

    /*Se crea la petición con los datos*/

    strcpy(p.q_name, q_name);
    p.op = 4;
    p.key = key;

    /*Se envia la petición y espera a la respuesta*/

    mq_send (q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    /*Se cierran las colas y se elimina la cola del cliente*/

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(q_name);

    /*Resultado de la operación*/

    return r.status;
}
int exist(int key)
{

    struct peticion p;
    struct respuesta r;
    char q_name[MAX_QUEUE];

    /*Apertura de colas de servidor y cliente con sus atribuitos y permisos*/

    sprintf(q_name, "%s%d", "/client_", getpid());      /*Se crea el nombre de la cola del cliente con su pid*/
    q_server = mq_open("/server", O_WRONLY, &q_attrs);
    if (q_server == -1)
    {
        perror("Error al abrir la cola del servidor");
        exit(-1);
    }
    q_client = mq_open(q_name, O_CREAT | O_RDONLY, 0644,&q_attrc);
    if (q_client == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }

    /*Se crea la petición con los datos*/

    strcpy(p.q_name, q_name);
    p.op = 5;
    p.key = key;

    /*Se envia la petición y espera a la respuesta*/

    mq_send (q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    /*Se cierran las colas y se elimina la cola del cliente*/

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(q_name);

    /*Resultado de la operación*/

    return r.status;
}
int copy_key(int key1, int key2)
{

    struct peticion p;
    struct respuesta r;
    char q_name[MAX_QUEUE];

    /*Apertura de colas de servidor y cliente con sus atribuitos y permisos*/

    sprintf(q_name, "%s%d", "/client_", getpid());      /*Se crea el nombre de la cola del cliente con su pid*/
    q_server = mq_open("/server", O_WRONLY, &q_attrs);
    if (q_server == -1)
    {
        perror("Error al abrir la cola del servidor");
        exit(-1);
    }
    q_client = mq_open(q_name, O_CREAT | O_RDONLY, 0644,&q_attrc);
    if (q_client == -1)
    {
        perror("Error al abrir la cola del cliente");
        exit(-1);
    }

    /*Se crea la petición con los datos*/

    strcpy(p.q_name, q_name);
    p.op = 6;
    p.key = key1;
    p.key2 = key2;

    /*Se envia la petición y espera a la respuesta*/

    mq_send (q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    /*Se cierran las colas y se elimina la cola del cliente*/

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(q_name);

    /*Resultado de la operación*/

    return r.status;
}