/*standard libraries*/

#include<string.h>
#include<stdlib.h>
#include<stdio.h>

/*libraries for threads*/

#include <pthread.h>

#include "servidor.h"

#define MAX_CHAIN 255 //tamaño maximo de la cadena

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
    int key;
    char value1[MAX_CHAIN];    
    int value2; //valor 2
    double value3; //valor 3
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

int real_exist(int key) 
{
        
/*Este servicio permite determinar si existe un elemento con clave key.
La función devuelve 1 en caso de que exista y 0 en caso de que no exista. En caso de error se
devuelve -1. Un error puede ocurrir en este caso por un problema en las comunicaciones*/

        /*Fase lector*/
        /*Abre lector*/

        pthread_mutex_lock(&read_mutex);
        n_readers++;
        long int th_id;
        if (n_readers == 1){
                th_id = pthread_self();

                /*Cierra escritor*/

                pthread_mutex_lock(&source_mutex);
        }
        pthread_mutex_unlock(&read_mutex);

        /*Abre lector*/
        /*Se leen los datos*/
        /*Si no existe una cabez, linked list vacia, no existe la key*/

        if (head == NULL){

                /*Cierra lector*/

                pthread_mutex_lock(&read_mutex);
                n_readers--;

                /*Si es el thread correspondiente*/

                if ( th_id == pthread_self()){

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

                /*Se abre lector*/
                /*Devuelve que no existe la clave*/

                return -1;
        }

        /*En caso de no estar la linked list vacia*/

        struct nodo *aux3;
        aux3 = head;

        /*Se recorre la linked list en busca de la clave*/

        while(aux3->next != NULL){
                if(aux3->key == key){
                        pthread_mutex_lock(&read_mutex);
                        n_readers--;

                        /*Si es el thread correspondiente*/

                        if ( th_id == pthread_self()){

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

                        /*Se abre lector*/
                        /*Devuelve que existe la clave*/

                        return 0;
                }
                aux3 = aux3->next;
        }

        /*Se revisa el ultimo nodo o head*/

        if(aux3->key == key){
                pthread_mutex_lock(&read_mutex);
                n_readers--;

                /*Si es el thread correspondiente*/

                if ( th_id == pthread_self()){

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

                /*Se abre lector*/
                /*Devuelve que existe la clave*/

                return 0;
        }
        pthread_mutex_lock(&read_mutex);
        n_readers--;

        /*Si es el thread correspondiente*/

        if ( th_id == pthread_self()){

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
        
        /*Se abre lector*/
        /*Devuelve que no existe la clave*/
        
        return -1;
}

int real_init()
{

/* Esta llamada permite inicializar el servicio de elementos clave-valor1-valor2-
valor3. Mediante este servicio se destruyen todas las tuplas que estuvieran almacenadas
previamente. La función devuelve 0 en caso de éxito y -1 en caso de error.*/

        /*Fase escritor*/
        /*Abre escritor*/

        pthread_mutex_lock(&source_mutex);

        /*Si ya esta vacía no hay nada que hacer, status:0*/

        if (head == NULL){
                pthread_mutex_unlock(&source_mutex);
                return 0;
        }

        /*Se recorre la linked list y se liberan los nodos*/

        while(head->next != NULL){
                aux = head;
                head = head->next;
                free(aux);
        }
        
        /*Se libera el ultimo nodo*/

        free(head);
        head=NULL;
        pthread_mutex_unlock(&source_mutex);

        /*Se cierra escritor*/

        return 0;

}

int real_set_value(int key, char *value1, int value2, double value3)
{

/*Este servicio inserta el elemento <key, value1, value2, value3>. El servicio devuelve 0 si se
insertó con éxito y -1 en caso de error. Se considera error, intentar insertar una clave que ya
existe previamente. En este caso se devolverá -1 y no se insertará. También se considerará error
cualquier error en las comunicaciones.*/

        /*Concurrencia híbrida*/
        /*Fase lector*/

        if (real_exist(key) == 0){
                return -1;
        }

        /*Fase escritor*/
        /*Se abre escritor*/

        pthread_mutex_lock(&source_mutex);

        /*Si la linked list esta vacia, se crea el primer nodo*/

        if (head == NULL){ 
                head = (struct nodo*)malloc(sizeof(struct nodo));
                head->key = key;
                strcpy(head->value1, value1);
                head->value2 = value2;
                head->value3 = value3;
                head->next = NULL;
                pthread_mutex_unlock(&source_mutex);

                /*Se cierra escritor*/

                return 0;
        }
        else    /*Si la linked list no esta vacia, se crea un nuevo nodo al final*/
        {
                aux = (struct nodo*)malloc(sizeof(struct nodo));
                aux->key = key;
                strcpy(aux->value1, value1);
                aux->value2 = value2;
                aux->value3 = value3;
                aux->next = NULL;
                aux2=head;
                while(aux2->next!=NULL)
                {
                        aux2=aux2->next;
                }
                aux2->next=aux;
                pthread_mutex_unlock(&source_mutex);

                /*Se cierra escritor*/

                return 0;
        }
        
}

int real_get_value(int key, char *value1, int *value2, double *value3) 
{

/*Este servicio permite obtener los valores asociados a la clave key. Los valores se devuelven en
value1, value2 y value3. La función devuelve 0 en caso de éxito y -1 en caso de error, por
ejemplo, si no existe un elemento con dicha clave o si se produce un error de comunicaciones*/

        int res=-1;
        struct nodo *aux3 = NULL;

        /*Fase lector*/

        if(real_exist(key)==-1)
        {
                return -1; /*La key no existe*/
        }
        
        /*Se abre lector*/

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
        while(aux3->next != NULL){
                if(aux3->key == key){
                        res = 0;
                        break;
      
                }
                aux3 = aux3->next;
        }

        /*Se revisa el ultimo nodo o head*/

        if(aux3->key == key){
                res = 0;          
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

        /*Se abre lector*/
        /*Se copian los valores*/

        if (aux3 != NULL && res==0){ 
                strcpy(value1, aux3->value1);
                *value2 = aux3->value2;
                *value3 = aux3->value3;
                return 0;
        }
        return -1;

}

int real_modify_value(int key, char *value1, int value2, double value3)
{

/*Este servicio permite modificar los valores asociados a la clave key. La función devuelve 0 en caso de
éxito y -1 en caso de error, por ejemplo, si no existe un elemento con dicha clave o si se produce
un error enlas comunicaciones.*/

        /*Concurrencia híbrida*/

        struct nodo *aux3 = NULL;
        int res;

        /*Fase lector*/

        if(real_exist(key)==-1)
        {
                return -1; /*La key no existe*/
        }

        /*Se abre lector*/

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
        while(aux3->next != NULL){
                if(aux3->key == key){
                        res = 0;
                        break;
      
                }
                aux3 = aux3->next;
        }

        /*Se revisa el ultimo nodo o head*/

        if(aux3->key == key){
                res = 0;            
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

        /*Se abre lector*/
        /*Fase escritor*/
        /*Se cierra escritor*/
        
        pthread_mutex_lock(&source_mutex);

        /*Se copian los valores*/
        
        if( aux3 != NULL && res == 0){
                strcpy(aux3->value1, value1);
                aux3->value2 = value2;
                aux3->value3 = value3;

                /*Se cierra escritor*/

                pthread_mutex_unlock(&source_mutex);
                return 0;
        }

        /*Se cierra escritor*/

        pthread_mutex_unlock(&source_mutex);
        return -1;
}

int real_delete_key(int key)
{

/*Este servicio permite borrar el elemento cuya clave es key. La
función devuelve 0 en caso de éxito y -1 en caso de error. En caso de que la clave no exista
también se devuelve -1.*/
        
        /*Concurrencia híbrida*/
        /*Fase lector*/

        struct nodo *aux3;
        struct nodo *aux4;
        int res;
        res = real_exist(key);

        /*Se cierra escritor*/

        pthread_mutex_lock(&source_mutex);
        if( res == 0){

                /*Se recorre la linked list buscando la key*/

                aux3 = head;

                /*Si solo tiene un nodo*/

                if (aux3->next==NULL)
                {
                        free(aux3);
                        head=NULL;
                        
                }

                /*Si es la cabeza se pasa la cabeza al siguiente y se libera*/

                if (aux3->key == key){
                        head = aux3->next;
                        free(aux3);
                        pthread_mutex_unlock(&source_mutex);
                        return 0;
                }

                /*Si no es la cabeza se recorre la lista*/

                while(aux3->next != NULL){

                        /*Si es la cola de la linked list*/

                        if (aux3->next->key == key && aux3->next->next == NULL){

                                aux4 = aux3->next;
                                aux3->next = NULL;
                                free(aux4);
                                break;
                        }

                        /*Si es un nodo intermedio*/

                        if(aux3->next->key == key){
                                aux4 = aux3; /*previous*/
                                aux3=aux3->next; /*actual*/
                                aux4->next = aux3->next;
                                free(aux3);
                                break;
                        }

                        aux3 = aux3->next;
                }

                /*Se cierra escritor*/

                pthread_mutex_unlock(&source_mutex);
                return 0;
        }

        /*Se cierra escritor*/

        pthread_mutex_unlock(&source_mutex);
        return -1;

}

int real_copy_key(int key1, int key2)
{

/*Este servicio crea e inserta una nueva clave (key2)
copiando los valores de la clave key1 en la nueva clave creada (key2). En caso de que key1 no
exista se devolverá -1 y no se creará la nueva clave. Si la clave key2 no existe, se creará y se
copiarán los valores de key1 en key2. En caso de que la clave key2 exista, se modificarán sus
valores a partir de los de key1. La función devuelve 0 en caso de poder crear y copiar los nuevos
datos en key2 y -1 en cualquier otro caso.*/

        /*Concurrencia híbrida*/
        /*Fase lector*/
        
        struct nodo *aux3 = NULL;
        int res;
        if (real_exist(key1) == -1)
        {      
                return -1;                
        }

        /*Se abre lector*/

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
        while(aux3->next != NULL){
                if(aux3->key == key1){
                        res = 0;
                        break;
                }
                aux3 = aux3->next;
        }

        /*Se revisa el ultimo nodo o head*/

        if(aux3->key == key1){
                res = 0;            
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

        /*Se abre lector*/
        /*Fase escritor*/

        if(aux3 != NULL && res == 0){

                /*Si la clave no existe se inserta como nueva*/

                if(real_exist(key2)==-1)
                {
                        
                        real_set_value(key2,aux3->value1,aux3->value2,aux3->value3);
                        return 0;

                } else          /*Si ya existe se modifica*/
                {

                        real_modify_value(key2, aux3->value1, aux3->value2, aux3->value3);
                        return 0; 

                }

        }
        return -1;


}
