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
#include "tuplas.h"

int sd;

#define MAX_CHAIN 255 //tamaño maximo de la cadena
#define MAX_SEND 1024 //tamaño maximo de la cadena de envio

int init()
{ ​

    char *host = getenv("IP_TUPLAS");
    
    if (host == NULL){
        perror("ERROR: IP no encontrado\n");
    }

    CLIENT *clnt;
    enum clnt_stat retval_1;
    int result_1;

    clnt = clnt_create (host, tuples, TUPLESVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (host);
        exit (1);
    }

    retval_1 = initto_429005(&result_1, clnt);
    if (retval_1 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    }

    return result_1;
}

int set_value(int key, char *value1, int value2, double value3)
{
    char *host = getenv("IP_TUPLAS");
    
    if (host == NULL){
        perror("ERROR: IP no encontrado\n");
    }

    CLIENT *clnt;
    enum clnt_stat retval_2;
    int result_2;
    clnt = clnt_create (host, tuples, TUPLESVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (host);
        exit (1);
    }

    retval_2 = settovarue_429005(key,value1,value2,value3,&result_2, clnt);
    if (retval_2 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    }
    return result_2;
}

int get_value(int key, char *value1, int *value2, double *value3)
{
    char *host = getenv("IP_TUPLAS");
    
    if (host == NULL){
        perror("ERROR: IP no encontrado\n");
    }

    CLIENT *clnt;
    enum clnt_stat retval_3;
    int result_3;
    struct Respuesta Res;
    clnt = clnt_create (host, tuples, TUPLESVER, "tcp");
    retval_3 = gettovarue_429005(&key,Res, clnt);
    result_3=key;
    if (retval_3 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    }
    return result_3;
}

int modify_value(int key, char *value1, int value2, double value3)
{

    char *host = getenv("IP_TUPLAS");
    
    if (host == NULL){
        perror("ERROR: IP no encontrado\n");
    }

    CLIENT *clnt;
    enum clnt_stat retval_4;
    int result_4;
    clnt = clnt_create (host, tuples, TUPLESVER, "tcp");
    retval_4 = modifuicvarue_429005(key,value1,value2,value3,&result_4, clnt);
    if (retval_4 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    }
    return result_4;

} 
int delete_key(int key)
{
    char *host = getenv("IP_TUPLAS");
    
    if (host == NULL){
        perror("ERROR: IP no encontrado\n");
    }

    CLIENT *clnt;
    enum clnt_stat retval_5;
    int result_5;
    clnt = clnt_create (host, tuples, TUPLESVER, "tcp");
    retval_5 = deletto_429005(key,&result_5, clnt);
    if (retval_5 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    }
    return result_5;
}
int exist(int key)
{
    char *host = getenv("IP_TUPLAS");
    
    if (host == NULL){
        perror("ERROR: IP no encontrado\n");
    }

    CLIENT *clnt;
    enum clnt_stat retval_6;
    int result_6;
    clnt = clnt_create (host, tuples, TUPLESVER, "tcp");
    retval_6 = existt_429005(key,&result_6, clnt);
    if (retval_6 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    }
    return result_6;
}

int copy_key(int key1, int key2)
{
char *host = getenv("IP_TUPLAS");
    
    if (host == NULL){
        perror("ERROR: IP no encontrado\n");
    }

    CLIENT *clnt;
    enum clnt_stat retval_7;
    int result_7;
    clnt = clnt_create (host, tuples, TUPLESVER, "tcp");
    retval_7 = existt_429005(key,key2,&result_7, clnt);
    if (retval_7 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    }
    return result_6;
}
