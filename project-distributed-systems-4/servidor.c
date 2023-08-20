/*standard libraries*/

#include<string.h>
#include<stdlib.h>
#include<stdio.h>

/*libraries for threads*/

#include <pthread.h>

#include "servidor.h"

#define MAX_CHAIN 255 //tamaño maximo de la cadena)
#define P_MSG 518 //tamaño de la estructura mensaje pendiente

struct pending_message
{
        int id;
        char alias2[MAX_CHAIN];
        char message[MAX_CHAIN];
        struct pending_message *next;
};

/*Para la linked list*/

struct nodo *head = NULL;
struct nodo *aux = NULL;
struct nodo *aux2 = NULL;

/*Creación de mutex, condiciones y variables para concurrencia en servidor*/
/*Lector y escritor con prioridad a lector*/

pthread_mutex_t read_mutex;
pthread_mutex_t source_mutex;
pthread_cond_t read_cond;
int n_readers = 0;


/*Estructura del nodo para linked list*/
struct nodo
{
        char name[MAX_CHAIN];
        char alias[MAX_CHAIN];    
        char date[MAX_CHAIN]; 
        char state[MAX_CHAIN];
        char IP[MAX_CHAIN];
        int port;
        int id_counter;
        struct pending_message *pending; 
        struct nodo *next;
};

/*Función para inicializar los mutex de concurrencia desde proxy*/

int initialize_mutex(){
        pthread_mutex_init(&read_mutex, NULL);
        pthread_mutex_init(&source_mutex, NULL);
        pthread_cond_init(&read_cond, NULL);
        return 1;
        
}

/*Función para destruir los mutex de concurrencia desde proxy*/

void destroy_mutex(){
        pthread_mutex_destroy(&read_mutex);
        pthread_mutex_destroy(&source_mutex);
        pthread_cond_destroy(&read_cond);

}

int register_client(char *name, char *alias, char *date)
{

/**/

        /*Fase escritor*/
        /*Se abre escritor*/

        pthread_mutex_lock(&source_mutex);

        /*Si la linked list esta vacia, se crea el primer nodo*/

        if (head == NULL){ 
                head = (struct nodo*)malloc(sizeof(struct nodo));
                strcpy(head->name, name);
                strcpy(head->alias, alias);
                strcpy(head->date, date);
                strcpy(head->state, "Desconectado");
                strcpy(head->IP, "NULL");
                head->port = 0;
                head->id_counter = 0;
                head->next = NULL;
                pthread_mutex_unlock(&source_mutex);

                /*Se cierra escritor*/

                return 0;
        }
        else    /*Si la linked list no esta vacia, se crea un nuevo nodo al final*/
        {
                
                aux = (struct nodo*)malloc(sizeof(struct nodo));
                strcpy(aux->name, name);
                strcpy(aux->alias, alias);
                strcpy(aux->date, date);
                strcpy(aux->state, "Desconectado");
                strcpy(aux->IP, "NULL");
                aux->port = 0;
                aux->id_counter = 0;
                aux->next = NULL;
                aux2=head;
                while(aux2->next!=NULL)
                {
                        if (strcmp(aux2->alias, alias) == 0){
                                pthread_mutex_unlock(&source_mutex);

                                /*Se cierra escritor*/

                                return 1;
                        }
                        aux2=aux2->next;
                }
                if (strcmp(aux2->alias, alias) == 0){
                                pthread_mutex_unlock(&source_mutex);

                                /*Se cierra escritor*/

                                return 1;
                        }
                aux2->next=aux;
                pthread_mutex_unlock(&source_mutex);

                /*Se cierra escritor*/

                return 0;
        }
        
}

int connect_disconnect(char *alias, char *status, char *IP, int port, char **set_messages, int *count)
{

/**/

        /*Concurrencia híbrida*/

        struct nodo *aux3 = NULL;
        struct pending_message *pend3 = NULL;
        int counter = 0;
        int i = 0;

        /*Se recorre la linked list buscando el alias*/

        pthread_mutex_lock(&source_mutex);

        aux3 = head;
        if (aux3 == NULL){
                pthread_mutex_unlock(&source_mutex);
                return 1;
        }

        while(aux3->next != NULL){
                if(strcmp(aux3->alias, alias) == 0){
                        break;
      
                }
                aux3 = aux3->next;
                i++;
        }

        /*Se revisa el ultimo nodo o head*/
        if(strcmp(aux3->alias, alias) == 0){
                if (strcmp(aux3->state, "Desconectado") == 0 && strcmp(status, "Conectado") == 0) /*Connect client*/
                {
                        strcpy(aux3->state,status);
                        strcpy(aux3->IP, IP);
                        aux3->port = port;
                        pend3 = aux3->pending;
                        if(pend3 == NULL){
                                pthread_mutex_unlock(&source_mutex);
                                *count = counter;
                                return 0;
                        }
                        while(pend3->next != NULL){
                                counter++;
                                pend3 = pend3->next;
                        }
                        counter++;
                        *count = counter;
                        *set_messages = malloc(P_MSG * counter);
                        int i = 0;
                        char msg[P_MSG];
                        char msg_id[sizeof(int)];
                        while(pend3->next != NULL){
                                strcat(msg, pend3->alias2);
                                strcat(msg, ";");
                                sprintf(msg_id, "%d", pend3->id);
                                strcpy(msg, msg_id);
                                strcat(msg, ";");
                                strcat(msg, pend3->message);
                                strcpy(*set_messages + i * P_MSG, msg);
                                i++;
                                pend3 = pend3->next;
                        }

                        strcat(msg, pend3->alias2);
                        strcat(msg, ";");
                        sprintf(msg_id, "%d", pend3->id);
                        strcpy(msg, msg_id);
                        strcat(msg, ";");
                        strcat(msg, pend3->message);
                        strcpy(*set_messages + i * P_MSG, msg);    
                        pthread_mutex_unlock(&source_mutex);
                        return 0;

                } else if (strcmp(aux3->state, "Conectado") == 0 && strcmp(status, "Desconectado") == 0) /*Disconnect client*/
                {
                        strcpy(aux3->state,status);
                        strcpy(aux3->IP, IP);
                        aux3->port = port;
                        pthread_mutex_unlock(&source_mutex);
                        return 0;
                } else if (strcmp(aux3->state, "Conectado") == 0 && strcmp(status, "Conectado") == 0) /*Connect client when connected*/
                {
                        pthread_mutex_unlock(&source_mutex);
                        return 2;
                } else if (strcmp(aux3->state, "Desconectado") == 0 && strcmp(status, "Desconectado") == 0) /*Disconnect client when disconnected*/
                {
                        pthread_mutex_unlock(&source_mutex);
                        return 2;
                }
        }
        pthread_mutex_unlock(&source_mutex);
        return 1;
}

int unregister_client(char *alias)
{

/**/
        /*Fase lector*/

        struct nodo *aux3;
        struct nodo *aux4;

        /*Se cierra escritor*/

        pthread_mutex_lock(&source_mutex);

        /*Se recorre la linked list buscando el alias*/

        aux3 = head;
        if (aux3 == NULL){
                pthread_mutex_unlock(&source_mutex);
                return 1;
        }

        /*Si solo tiene un nodo*/

        if (aux3->next == NULL && strcmp(aux3->alias, alias) == 0)
        {
                free(aux3);
                head=NULL;
                pthread_mutex_unlock(&source_mutex);
                return 0;
                
        }

        /*Si es la cabeza se pasa la cabeza al siguiente y se libera*/

        if (strcmp(aux3->alias, alias) == 0){
                head = aux3->next;
                free(aux3);
                pthread_mutex_unlock(&source_mutex);
                return 0;
        }

        /*Si no es la cabeza se recorre la lista*/

        while(aux3->next != NULL){

                /*Si es la cola de la linked list*/

                if (strcmp(aux3->next->alias, alias) == 0 && aux3->next->next == NULL){

                        aux4 = aux3->next;
                        aux3->next = NULL;
                        free(aux4);
                        /*Se cierra escritor*/
                        pthread_mutex_unlock(&source_mutex);
                        return 0;
                }

                /*Si es un nodo intermedio*/

                if(strcmp(aux3->next->alias, alias) == 0){
                        aux4 = aux3; /*previous*/
                        aux3=aux3->next; /*actual*/
                        aux4->next = aux3->next;
                        free(aux3);
                        /*Se cierra escritor*/
                        pthread_mutex_unlock(&source_mutex);
                        return 0;
                        
                }

                aux3 = aux3->next;
        }

        /*Se cierra escritor*/
        pthread_mutex_unlock(&source_mutex);
        return 1;

}

int clients_connected_request(char *alias, char **connected_clts, int *count)
{

        struct nodo *aux3 = NULL;
        int counter = 0;
        pthread_mutex_lock(&read_mutex);
        n_readers++;
        long int th_id;
        if (n_readers == 1){
                th_id = pthread_self();

                /*Se cierra escritor*/

                pthread_mutex_lock(&source_mutex);
        }
        pthread_mutex_unlock(&read_mutex);

        /*Se cierra lector*/
        /*Se recorre la linked list buscando la key*/
        
        aux3 = head;
        if (aux3 == NULL){
                pthread_mutex_unlock(&source_mutex);
                return 2;
        }

        while(aux3->next != NULL){
                if (strcmp(aux3->alias, alias) == 0){
                        break;
                }
                aux3 = aux3->next;
        }
        if (strcmp(aux3->alias, alias) == 0){
                if (strcmp(aux3->state, "Desconectado") == 0){
                        /*Se cierra lector*/

                        pthread_mutex_lock(&read_mutex);
                        n_readers--;

                        /*Si es el thread correspondiente*/

                        if (th_id == pthread_self()){

                                /*Espera a que no haya mas lectores*/

                                while (n_readers > 0){
                                        pthread_cond_wait(&read_cond, &read_mutex);
                                }

                                /*Abre escritor*/

                                pthread_mutex_unlock(&source_mutex);
                        
                        } else {
                                pthread_cond_signal(&read_cond);
                        }
                        pthread_mutex_unlock(&read_mutex);
                        return 1;
                }
        } else {
                /*Se cierra lector*/

                pthread_mutex_lock(&read_mutex);
                n_readers--;

                /*Si es el thread correspondiente*/

                if (th_id == pthread_self()){

                        /*Espera a que no haya mas lectores*/

                        while (n_readers > 0){
                                pthread_cond_wait(&read_cond, &read_mutex);
                        }

                        /*Abre escritor*/

                        pthread_mutex_unlock(&source_mutex);
                
                } else {
                        pthread_cond_signal(&read_cond);
                }
                pthread_mutex_unlock(&read_mutex);
                return 2;
        }
        aux3 = head;
        while(aux3->next != NULL){
                if (strcmp(aux3->state, "Conectado") == 0){
                        counter++;      
                }
                aux3 = aux3->next;
        }
        if (strcmp(aux3->state, "Conectado") == 0){
                counter++;
        }
        *count = counter;
        *connected_clts = malloc((MAX_CHAIN * counter) + counter - 1);
        int i = 0;
        char user[MAX_CHAIN+1];
        aux3 = head;
        while(aux3->next != NULL){
                if (strcmp(aux3->state, "Conectado") == 0){
                        strcpy(user, aux3->alias);
                        strcat(user, ",");
                        strcat(*connected_clts + i*MAX_CHAIN, user);
                }
                aux3 = aux3->next;
                i++;
        }
        if (strcmp(aux3->state, "Conectado") == 0){
                strcat(*connected_clts + i*MAX_CHAIN, aux3->alias);
        }

        /*Se cierra lector*/

        pthread_mutex_lock(&read_mutex);
        n_readers--;

        /*Si es el thread correspondiente*/

        if (th_id == pthread_self()){

                /*Espera a que no haya mas lectores*/

                while (n_readers > 0){
                        pthread_cond_wait(&read_cond, &read_mutex);
                }

                /*Abre escritor*/

                pthread_mutex_unlock(&source_mutex);
        
        } else {
                pthread_cond_signal(&read_cond);
        }
        pthread_mutex_unlock(&read_mutex);
        return 0; 

}

int send_message(char *alias, char *alias2, char *message, char *IP, int *port, char *alias2_status, int *m_id, char *user_IP, int *user_port)
{

        /**/
        
        char status_alias [MAX_CHAIN];
        char status_alias2 [MAX_CHAIN];
        pthread_mutex_lock(&source_mutex);

        struct nodo *aux3 = NULL;
        struct nodo *aux4 = NULL;
        struct nodo *aux5 = NULL;

        aux3 = head;
        if (aux3 == NULL){
                pthread_mutex_unlock(&source_mutex);
                return 1;
        }
        
        while(aux3->next != NULL){
                if(strcmp(aux3->alias, alias) == 0){
                        aux5 = aux3;
                        strcpy(status_alias, aux3->state);
                }
                if(strcmp(aux3->alias, alias2) == 0){
                        aux4 = aux3;
                        strcpy(status_alias2, aux3->state);
                }
                aux3 = aux3->next;
        }
        if(strcmp(aux3->alias, alias) == 0){
                aux5 = aux3;
                strcpy(status_alias, aux3->state);
        }
        if(strcmp(aux3->alias, alias2) == 0){
                strcpy(status_alias2, aux3->state);
                aux4 = aux3;
        }

        if (strlen(status_alias) == 0 || strlen(status_alias2) == 0){
                pthread_mutex_unlock(&source_mutex);
                return 2;
        }

        /*Se revisa el ultimo nodo o head*/
                
        if(strcmp(status_alias2, "Desconectado") == 0){ /*Disconnected*/

                printf("Entro al if correspondiente\n");
                
                if (aux4->pending == NULL){
                        aux4->pending = (struct pending_message *)malloc(sizeof(struct pending_message));
                        aux4->pending->id = aux5->id_counter;
                        aux5->id_counter++;
                        strcpy(aux4->pending->alias2, alias);
                        strcpy(aux4->pending->message, message);
                        aux4->pending->next = NULL;
                } else {
                        struct pending_message *aux3 = NULL;
                        struct pending_message *aux6 = NULL;
                        aux3 = aux4->pending;
                        while(aux3->next != NULL){
                                aux3 = aux3->next;
                        }
                        aux6 = (struct pending_message *)malloc(sizeof(struct pending_message));
                        aux6->id = aux5->id_counter;
                        aux5->id_counter++;
                        strcpy(aux6->alias2, alias);
                        strcpy(aux6->message, message);
                        aux6->next = NULL;
                        aux3->next = aux6;

                }
                strcpy(alias2_status, status_alias2);
                printf("Antes del unlock\n");

                pthread_mutex_unlock(&source_mutex);
                printf("Despues del unlock\n");
                return 0;
                
        }else{
                
                strcpy(alias2_status, status_alias2);
                *m_id = aux5->id_counter;
                strcpy(IP, aux4->IP);
                *port = aux4->port;
                aux5->id_counter++;
                strcpy(user_IP, aux5->IP);
                *user_port = aux5->port;
                pthread_mutex_unlock(&source_mutex);
                return 0;

        }

        /*Se cierra escritor*/

        pthread_mutex_unlock(&source_mutex);

        return 1;

}

int not_sent_message (char *alias, char *alias2, char *message, int id_mssg)
{

        pthread_mutex_lock(&source_mutex);
        struct nodo *aux3 = NULL;
        struct pending_message *pend3 = NULL;
        struct pending_message *pend4 = NULL;

        aux3 = head;
        if (aux3 == NULL){
                pthread_mutex_unlock(&source_mutex);
                return 1;
        }

        while(aux3->next != NULL){
                if(strcmp(aux3->alias, alias) == 0){
                        break;
                }
                aux3 = aux3->next;
        }
        if(strcmp(aux3->alias, alias) == 0){
               pend3 = aux3->pending;

               if (pend3 == NULL){
                        strcpy(pend3->alias2, alias2);
                        pend3->id = id_mssg;
                        strcpy(pend3->message, message);
                        pend3->next = NULL;
                        pthread_mutex_unlock(&source_mutex);
                        return 0;
                }

                while(pend3->next != NULL){

                        pend3 = pend3->next;
                }
               
                strcpy(pend4->alias2, alias2);
                pend4->id = id_mssg;
                strcpy(pend4->message, message);
                pend4->next = NULL;
                pend3->next = pend4;
                pthread_mutex_unlock(&source_mutex);
                return 0;
        }
        pthread_mutex_unlock(&source_mutex);
        return 1;
}