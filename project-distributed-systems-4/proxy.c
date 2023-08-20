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
#include "lines.h"

#define TRUE 1
#define FALSE 0

#define MAX_CHAIN 255 //tamaño maximo de la cadena
#define MAX_SEND 1024 //tamaño maximo de la cadena a recibir
#define P_MSG 518

/*Mantenemos la estructura de peticion para tratarla*/

struct peticion 
{
    int socket_cliente;
    struct sockaddr_in cliente_addr;
};

/*Mutex y variables para la sincronizacion*/

pthread_mutex_t sync_mutex;
int sync_copied = FALSE;
pthread_cond_t sync_cond; 

/*Funcion para tratar las peticiones, llamada a servidor.c que trata los datos*/

void * tratar_peticion(void *sckt_pttn)
{

    int op;
    int sc;
    char clientIP[INET_ADDRSTRLEN];
    struct sockaddr_in client_addr;

    /*Datos*/

    char operation[MAX_CHAIN]; 
    char name[MAX_CHAIN]; 
    char alias[MAX_CHAIN];
    char alias2[MAX_CHAIN]; 
    char date[MAX_CHAIN];
    char IP[MAX_CHAIN];
    char state[MAX_CHAIN];
    char puerto[MAX_CHAIN]; 
    int port; 
    char message[MAX_CHAIN]; 

    /*Sincronizacion*/

    pthread_mutex_lock(&sync_mutex);
    sc = ((struct peticion *)sckt_pttn)->socket_cliente;
    client_addr = ((struct peticion *)sckt_pttn)->cliente_addr;
    readLine(sc, (char *)&operation, MAX_CHAIN);
    if (strcmp(operation,"REGISTER") == 0){
        op = 0;
        readLine(sc, (char *)&name, MAX_CHAIN);
        readLine(sc, (char *)&alias, MAX_CHAIN);
        readLine(sc, (char *)&date, MAX_CHAIN);
    } else if(strcmp(operation,"UNREGISTER") == 0){
        op = 1;
        readLine(sc, (char *)&alias, MAX_CHAIN);
    } else if(strcmp(operation,"CONNECT") == 0){
        op = 2;
        readLine(sc, (char *)&alias, MAX_CHAIN);
        readLine(sc, (char *)&puerto, MAX_CHAIN);
        port = atoi(puerto);
        strcpy(state,"Conectado");
        inet_ntop(AF_INET, &(client_addr.sin_addr), clientIP, INET_ADDRSTRLEN);
        strcpy(IP, clientIP);
    } else if(strcmp(operation,"DISCONNECT") == 0){
        op = 2;
        readLine(sc, (char *)&alias, MAX_CHAIN);
        strcpy(IP, "NULL");
        strcpy(state,"Desconectado");
        port = 0;
    } else if(strcmp(operation,"CONNECTEDUSERS") == 0){
        op = 4;
        readLine(sc, (char *)&alias, MAX_CHAIN);
    } else if(strcmp(operation,"SEND") == 0){
        op = 3;
        readLine(sc, (char *)&alias, MAX_CHAIN);
        readLine(sc, (char *)&alias2, MAX_CHAIN);
        readLine(sc, (char *)&message, MAX_CHAIN);
    }
    sync_copied = TRUE;
    pthread_cond_signal(&sync_cond);
    pthread_mutex_unlock(&sync_mutex);

    /*Variables para devolver los datos de get_value()*/
    
    int status;
    char res[MAX_SEND];
    char *res_connected_clts = NULL;
    int count = 0;
    char alias2_sts[MAX_CHAIN];
    char *set_messages = NULL;
    char res_IP[MAX_CHAIN];
    int res_port;
    char user_IP[MAX_CHAIN];
    int user_port;
    int m_id;

    /*Switch case para tratar las peticiones*/

    switch(op)
    {
        case 0: status = register_client(name, alias, date);
                sprintf(res,"%d", status);
                /*Se abre la cola del cliente y se envia la respuesta*/

                if (status == 0){
                    printf("s< REGISTER %s OK\n", alias);
                } else {
                    printf("s< REGISTER %s FAIL\n", alias);
                }

                /*Se envia la respuesta*/
                if (write(sc, (char *)&res, strlen(res)+1) == -1)
                {
                    perror("SERVER ERROR: Error al enviar la respuesta\n");
                    exit(-1);
                } 
                break;

        case 1: status = unregister_client(alias); 
                sprintf(res,"%d", status);
                /*Se abre la cola del cliente y se envia la respuesta*/

                if (status == 0){
                    printf("s< UNREGISTER %s OK\n", alias);
                } else {
                    printf("s< UNREGISTER %s FAIL\n", alias);
                }

                /*Se envia la respuesta*/

                if (write(sc, (char *)&res, strlen(res)+1) == -1)
                {
                    perror("SERVER ERROR: Error al enviar la respuesta\n");
                    exit(-1);
                } 
                break;

        case 2: status  = connect_disconnect(alias, state, IP, port, &set_messages, &count);
                sprintf(res,"%d", status);
                /*Se abre la cola del cliente y se envia la respuesta*/

                if (status == 0 && strcmp(IP, "NULL") != 0){
                    printf("s< CONNECT %s OK\n", alias);
                } else if (status != 0 && strcmp(IP, "NULL") != 0){
                    printf("s< CONNECT %s FAIL\n", alias);
                } else if (status == 0 && strcmp(IP, "NULL") == 0){
                    printf("s< DISCONNECT %s OK\n", alias);
                } else if (status != 0 && strcmp(IP, "NULL") == 0){
                    printf("s< DISCONNECT %s FAIL\n", alias);
                }

                /*Se envia la respuesta*/

                if (write(sc, (char *)&res, strlen(res)+1) == -1)
                {
                    perror("SERVER ERROR: Error al enviar la respuesta\n");
                    exit(-1);
                }

                /*Se envia el numero de mensajes*/
                if ( count > 0 )
                {

                    /*Apertura del socket del cliente thread y envias*/

                    int sd /*, sb*/ ;
                    struct sockaddr_in server_addr; /*user_addr*/
                    struct hostent *hp;
                    /*struct hostent *hp_user;*/
                    char *token;

                    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                    {
                        perror("SERVER ERROR: Error al abrir el socket\n");
                        exit(-1);
                    }

                    bzero((char *)&server_addr, sizeof(server_addr));
                    hp = gethostbyname(IP);

                    memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
                    server_addr.sin_family = AF_INET;
                    server_addr.sin_port = htons(port);

                    int err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
                    if (err == -1)
                    {
                        perror("SERVER ERROR: Error al conectar\n");
                        exit(-1);
                    }

                    char *mess = "SEND_MESSAGE";
                    /*char *mess_user = "SEND_MESS_ACK";*/ 

                    /*Solo se ejecuta con mensajes pendientes*/
                    for(int i = 0; i < count; i++)
                    {
                        
                        /*Se envia el mensaje elemento a elemento*/
                        strcpy(res, mess);

                        if (write(sd, (char *)&res, strlen(res)+1) == -1)
                        {
                            perror("SERVER ERROR: Error al enviar el mensaje\n");
                            not_sent_message (alias, alias2, message, m_id);    // En caso de error no se manda el mensaje
                            exit(-1);
                        }

                        strcpy(res, (set_messages + i*P_MSG));//*size

                        if (write(sd, (char *)&res, strlen(res)+1) == -1)
                        {
                            perror("SERVER ERROR: Error al enviar el mensaje\n");
                            not_sent_message (alias, alias2, message, m_id);    // En caso de error no se manda el mensaje
                            exit(-1);
                        }

                        /*Usamos strtok para tomas los elementos que nos devuelve servidor uno a uno*/

                        char id[50];
                        token = strtok(res, ";");
                        strcpy(alias, token);
                        while (token != NULL) {
                            token = strtok(NULL, ";");
                            strcpy(id, token);
                            token = strtok(NULL, ";");
                            strcpy(message, token);
                        }

                        /*Mandamos uno a uno los elementos que hemos conseguido en el strtok*/
                        if (write(sd, (char *)&alias, strlen(alias)+1) == -1)
                        {
                            perror("SERVER ERROR: Error al enviar el mensaje\n");
                            int id1 = atoi(id);
                            not_sent_message (alias, alias2, message, id1);
                            exit(-1);
                        }

                        if (write(sd, (char *)&id, strlen(id)+1) == -1)
                        {
                            perror("SERVER ERROR: Error al enviar el mensaje\n");
                            int id1 = atoi(id);
                            not_sent_message (alias, alias2, message, id1);
                            exit(-1);
                        }

                        if (write(sd, (char *)&message, strlen(message)+1) == -1)
                        {
                            perror("SERVER ERROR: Error al enviar el mensaje\n");
                            int id1 = atoi(id);
                            not_sent_message (alias, alias2, message, id1);
                            exit(-1);
                        }

                        close(sd);

                    }
                    /*liberamos memoria*/
                    free(set_messages);
                }

                break;

        case 3: status = send_message(alias, alias2 ,message, res_IP, &res_port ,alias2_sts, &m_id, user_IP, &user_port); /*Devolver el IP y eso para del que lo recibe para abrir bien el socket*/
                sprintf(res,"%d", status);
                /*Se abre la cola del cliente y se envia la respuesta*/

                if (status == 0 && strcmp(alias2_sts, "Conectado") == 0){
                    printf("s< SEND MESSAGE %d FROM %s TO %s\n", m_id, alias, alias2);
                } else if (status == 0 && strcmp(alias2_sts, "Desconectado") == 0){
                    printf("s< SEND MESSAGE %d FROM %s TO %s STORED\n", m_id, alias, alias2);
                }

                /*Se envia la respuesta*/

                if (write(sc, (char *)&res, strlen(res)+1) == -1)
                {
                    perror("SERVER ERROR: Error al enviar la respuesta\n");
                    exit(-1);
                }

                if (strcmp(alias2_sts, "Conectado") == 0 && status == 0)
                {
                    
                    /*Apertura de socket del cliente thread y envio del mensaje*/

                    int sd,sb;
                    struct sockaddr_in server_addr;
                    struct hostent *hp;

                    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                    {
                        perror("SERVER ERROR: Error al abrir el socket\n");
                        exit(-1);
                    }

                    bzero((char *)&server_addr, sizeof(server_addr));
                    hp = gethostbyname(res_IP);

                    memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
                    server_addr.sin_family = AF_INET;
                    server_addr.sin_port = htons(res_port);

                    int err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
                    if (err == -1)
                    {
                        perror("SERVER ERROR: Error al conectar\n");
                        exit(-1);
                    }

                    /*Se envia el mensaje elemento a elemento */
                    char *mes ="SEND_MESSAGE";
                    strcpy(res, mes);
                    if (write(sd, (char *)&res, strlen(res)+1) == -1)
                    {
                        perror("SERVER ERROR: Error al enviar el mensaje\n");
                        not_sent_message (alias, alias2, message, m_id);    // En caso de error no se manda el mensaje
                        exit(-1);
                    }
                    strcpy(res, alias);
                    if (write(sd, (char *)&res, strlen(res)+1) == -1)
                    {
                        perror("SERVER ERROR: Error al enviar el mensaje\n");
                        not_sent_message (alias, alias2, message, m_id);    // En caso de error no se manda el mensaje
                        exit(-1);
                    }

                    sprintf(res,"%d" ,m_id);
                    if (write(sd, (char *)&res, strlen(res)+1) == -1)
                    {
                        perror("SERVER ERROR: Error al enviar el mensaje\n");
                        not_sent_message (alias, alias2, message, m_id);    // En caso de error no se manda el mensaje
                        exit(-1);
                    }
                    strcpy(res, message);
                    if (write(sd, (char *)&res, strlen(res)+1) == -1)
                    {
                        perror("SERVER ERROR: Error al enviar el mensaje\n");
                        not_sent_message (alias, alias2, message, m_id);    // En caso de error no se manda el mensaje
                        exit(-1);
                    }

                    /*Cerramos el socket*/
                    close(sd);

                    /* Abrimos un segundo socket para realizar el SEND_MESS_ACK*/
                    if ((sb = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                    {
                        perror("SERVER ERROR: Error al abrir el socket\n");
                        exit(-1);
                    }

                    bzero((char *)&server_addr, sizeof(server_addr));
                    hp = gethostbyname(user_IP);

                    memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
                    server_addr.sin_family = AF_INET;
                    server_addr.sin_port = htons(user_port);

                    /*Nos conectamos al socket*/
                    err = connect(sb, (struct sockaddr *)&server_addr, sizeof(server_addr));
                    if (err == -1)
                    {
                        perror("SERVER ERROR: Error al conectar\n");
                        exit(-1);
                    }

                    char *mes_user ="SEND_MESS_ACK";
                    strcpy(res, mes_user);
                    /*Mandamos el mensaje SEND_MESS_ACK al socket para que sepa que operacion se va a realizar*/
                    if (write(sb, (char *)&res, strlen(res)+1) == -1)
                    {
                        perror("SERVER ERROR: Error al enviar el mensaje\n");
                        exit(-1);
                    }
                    /*Mandamos la id del mensaje*/
                    sprintf(res,"%d" ,m_id);
                    if (write(sb, (char *)&res, strlen(res)+1) == -1)
                    {
                        perror("SERVER ERROR: Error al enviar el mensaje\n");
                        exit(-1);
                    }

                    close(sb);

                }

                break;

        case 4: status = clients_connected_request(alias, &res_connected_clts, &count);
                sprintf(res,"%d", status);
                /*Se abre la cola del cliente y se envia la respuesta*/

                if (status == 0){
                    printf("s< CONNECTEDUSERS OK\n");
                } else {
                    printf("s< CONNECTEDUSERS FAIL\n");
                }

                /*Se envia la respuesta*/

                if (write(sc, (char *)&res, strlen(res)+1) == -1)
                {
                    perror("SERVER ERROR: Error al enviar la respuesta\n");
                    exit(-1);
                }

                if (status == 0){
                    
                    sprintf(res,"%d", count);

                    if (write(sc, (char *)&res, strlen(res)+1) == -1)
                    {
                        perror("SERVER ERROR: Error al enviar la respuesta\n");
                        exit(-1);
                    }

                    for (int i = 0; i < count; i++)
                    {
                        /*Se envia el mensaje*/
                        strcpy(res, (res_connected_clts + i*MAX_CHAIN));//size
                         if (i!= count-1 ){
                            int len = strlen(res);
                            res[len-1]='\0';
                        }
                        if (write(sc, (char *)&res, strlen(res)+1) < 0)
                        {
                            perror("SERVER ERROR: Error al enviar el mensaje\n");
                            exit(-1);
                        }

                    }

                    free(res_connected_clts);

                }

                break;                
        
    }

    /*Se cierra el socket*/

    close(sc);

    return NULL;

}

int main (int argc, char *argv[])
{

    /*Se comprueba que se ha pasado el puerto*/

    if (argc != 3){

        perror("SERVER ERROR: Puerto del servidor no especificado\n");

    }

    /*Sockets cliente y servidor*/

    int sd, sc;

    /*Estructura*/

    struct peticion sckt_pttn;

    /*Para los threads*/

    pthread_attr_t t_attr;
    pthread_t thid;

    /*Se inicializa el mutex, condicion y atributo*/

    pthread_mutex_init(&sync_mutex, NULL);
    pthread_cond_init(&sync_cond, NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    initialize_mutex();

    /*Socket*/

    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int val, err;
    short puerto;

    puerto = atoi(argv[2]);

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

        /*Se copia el socket y IP*/

        sckt_pttn.socket_cliente = sc;
        sckt_pttn.cliente_addr = client_addr;

        /*Se crea el hilo para tratar la peticion*/

        pthread_create(&thid, &t_attr, (void *)tratar_peticion, (void*)&sckt_pttn);

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