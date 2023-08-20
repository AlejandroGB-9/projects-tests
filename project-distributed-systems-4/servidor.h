#ifndef SERVER_H
#define SERVER_H

#define TRUE 1
#define FALSE 0

int initialize_mutex();
void destroy_mutex();
int register_client(char *name, char *alias, char *date);
int connect_disconnect(char *alias, char *status, char *IP, int port, char **set_messages, int *count);
int unregister_client(char *alias); 
int clients_connected_request(char *alias, char **connected_clts, int *count);
int send_message(char *alias, char *alias2, char *message, char *IP, int *port, char *alias2_status, int *m_id, char *user_IP, int *user_port);
int not_sent_message (char *alias, char *alias2, char *message, int id_mssg);

#endif
