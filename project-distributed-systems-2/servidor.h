#ifndef SERVER_H
#define SERVER_H

#define TRUE 1
#define FALSE 0

int initialize_mutex();
void destroy_mutex();
int real_init();
int real_set_value(int key, char *value1, int value2, double value3);
int real_get_value(int key, char *value1, int *value2, double *value3);
int real_modify_value(int key, char *value1, int value2, double value3);
int real_delete_key(int key); 
int real_exist(int key);
int real_copy_key(int key1, int key2);

#endif
