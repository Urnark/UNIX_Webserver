#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int ip_type, protocol_type;
struct sockaddr_in server_address;
int portnumber;
int server_socket;
int server_socket_opt;

typedef struct
{
    int socket;
    struct sockaddr_in client_address;
} Client;

void error(char * message);
int set_protocol (int i);
int set_ip_type (int i);
int set_port (int i);
int createSocket (int queuelength);
Client connectToClient(int* accept_connection);
void closeServer();
void setToNonBlocking();
void resetFlags();

#endif