/*standard libraries*/

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include"claves.h"

#define MAX_CHAIN 255 //tamaño maximo de la cadena

int main(void)
{

    int key, key2, value2, op, result;
    double value3;
    char value1[MAX_CHAIN];

    /* peticion de la operación a realizar y captura de datos*/

    printf("Seleccione el número de operación que desea realizar: \n\t0. Init\n\t1. Set Value\n\t2. Get Value\n\t3. Modify Value\n\t4. Delete Key\n\t5. Exist\n\t6. Copy Key\n");
    scanf("%d",&op);
    while (op < 0 || op > 6)
    {
        printf("Número de operacion no valido\n");
        printf("Seleccione el número de operación que desea realizar: \n\t0. Init\n\t1. Set Value\n\t2. Get Value\n\t3. Modify Value\n\t4. Delete Key\n\t5. Exist\n\t6. Copy Key\n");
        scanf("%d",&op);
    }

    /*llamada a la función correspondiente de la operación, tratado por claves.c*/

    switch(op){

        case 0:
            printf("Ha seleccionado la operación Init\n");
            result = init();
            printf("Se han borrado todas las claves\n");
            break;
        case 1:
            printf("Ha seleccionado la operación Set Value\n");
            printf("Introduzca el valor de la clave: ");
            if(scanf("%d",&key)<=0)
            {
                result = -1;
                printf("El valor de la clave introducido es incorrecto.\n");
                break;
            }
            printf("Introduzca el valor del primer campo: ");
            if(scanf("%s",value1)<=0 || (strlen(value1)>MAX_CHAIN) )
            {
                result = -1;
                printf("El valor del valor1 introducido es incorrecto.\n");
                break;
            }
            printf("Introduzca el valor del segundo campo: ");
            if(scanf("%d",&value2)<=0)
            {
                result = -1;
                printf("El valor del valor2 introducido es incorrecto.\n");
                break;
            }
            printf("Introduzca el valor del tercer campo: ");
            if(scanf("%lf",&value3)<=0)
            {
                result = -1;
                printf("El valor del valor3 introducido es incorrecto.\n");
                break;
            }
            result = set_value(key, value1, value2, value3);
            if (result == -1){
                printf("Error al introducir los valores, la clave ya existe\n");
                break;
            }
            printf("Los valores se han guardado correctamente\n");
            break;
        case 2:
            printf("Ha seleccionado la operación Get Value\n");
            printf("Introduzca el valor de la clave: ");
            if(scanf("%d",&key)<=0)
            {
                result = -1;
                printf("El valor de la clave introducido es incorrecto.\n");
                break;
            }
            result = get_value(key, value1, &value2, &value3);
            if (result == -1){
                printf("Error al obtener los valores, la clave no existe\n");
                break;
            }
            printf("Los resultados son:\n Valor1: %s\n Valor2: %d\n Valor3: %lf\n", value1, value2, value3);
            break;
        case 3:
            printf("Ha seleccionado la operación Modify Value\n");
            printf("Introduzca el valor de la clave: ");
            if(scanf("%d",&key)<=0)
            {
                result = -1;
                printf("El valor de la clave introducido es incorrecto.\n");
                break;
            }
            printf("Introduzca el valor del primer campo: ");
            if(scanf("%s",value1)<=0 || (strlen(value1)>MAX_CHAIN))
            {
                result = -1;
                printf("El valor del valor1 introducido es incorrecto.\n");
                break;
            }
            printf("Introduzca el valor del segundo campo: ");
            if(scanf("%d",&value2)<=0)
            {
                result = -1;
                printf("El valor del valor2 introducido es incorrecto.\n");
                break;
            }
            printf("Introduzca el valor del tercer campo: ");
            if(scanf("%lf",&value3)<=0)
            {
                result = -1;
                printf("El valor del valor3 introducido es incorrecto.\n");
                break;
            }
            result = modify_value(key, value1, value2, value3);
            if (result == -1){
                printf("Error al modificar los valores, la clave no existe\n");
                break;
            }
            printf("Los valores han sido modificados\n");
            break;
        case 4:
            printf("Ha seleccionado la operación Delete Key\n");
            printf("Introduzca el valor de la clave: ");
            if(scanf("%d",&key)<=0)
            {
                result = -1;
                break;
            }
            result = delete_key(key);
            if (result == -1){
                printf("Error al borrar la clave, la clave no existe\n");
                break;
            }
            printf("La clave ha sido borrada\n");
            break;
        case 5:
            printf("Ha seleccionado la operación Exist\n");
            printf("Introduzca el valor de la clave: ");
            if(scanf("%d",&key)<=0)
            {
                result = -1;
                printf("El valor de la clave introducido es incorrecto.\n");
                break;
            }
            result = exist(key);
            if (result == -1){
                printf("La clave no existe\n");
                break;
            }
            printf("La clave introducida existe\n");
            break;
        case 6:
            printf("Ha seleccionado la operación Copy Key\n");
            printf("Introduzca el valor de la clave 1: ");
            if(scanf("%d",&key)<=0)
            {
                result = -1;
                printf("El valor de la clave1 introducido es incorrecto.\n");
                break;
            }
            printf("Introduzca el valor de la clave 2: ");
            if(scanf("%d",&key2)<=0)
            {
                result = -1;
                printf("El valor de la clave2 introducido es incorrecto.\n");
                break;
            }
            result = copy_key(key, key2);
            if (result==-1)
            {
                printf("Error copiando las claves\n");
                break;
            }
            printf("Claves copiadas\n");
            break;
    }
    printf("Resultado de cliente: %d\n", result);
    return result;

}