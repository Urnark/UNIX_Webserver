#include "../include/socket.h"
#include <fcntl.h>
#include <errno.h>

void error(char * message)
{
    perror(message);
    exit(1);
}

//Sets the protocoll to either TCP (SOCK_STREAM) or UDP(SOCK_DGRAM)
int set_protocol (int i)
{
    if(i == 0){
        protocol_type = SOCK_STREAM;
    }else if(i == 1){
        protocol_type = SOCK_DGRAM;
    }else{
        error("Could not properly set the protocoltype.");
    }
    return(0);
}

//Sets the type either to IPv4(AF_INET) or IPv6(AF_INET)
int set_ip_type (int i)
{
    if(i == 0){
        ip_type = AF_INET;
    }else if(i == 1){
        ip_type = AF_INET6;
    }else{
        error("Could not properly set the IP_type.");
    }
    return(0);
}

//Sets the portnumber
int set_port (int i)
{
    if(( 0 < i) && (i < 65535)){
        portnumber = i;
    }else{
        error("That is not a valid port.");
    }
    return(0);
}

//Creates the server_socket and bind it to the port to listen.
int createSocket (int queuelength)
{
    if((server_socket = socket(ip_type, protocol_type, 0)) < 0){
        error("Could not create the socket_descriptor.");
    }

    int true = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) < 0){
        error("Could not set socket options.");
    }

    //if further settings neccessary use setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)?

	memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = ip_type;
    server_address.sin_port = htons(portnumber);
    server_address.sin_addr.s_addr = INADDR_ANY; //reads own IP address

    if((bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address))) < 0){
        error("Could not bind the socket.");
    }

    if((listen(server_socket, queuelength)) < 0){
        error("Server could not be set to listen.");
    }

    // Save socket flags
    server_socket_opt = fcntl(server_socket, F_GETFL, NULL);

    printf("Successfully created Server_Socket\n");  // delete if not needed.
    return 0;
}

//accept a client.
Client connectToClient(int* accept_connection)
{
    Client client;
    int addrlen = sizeof(client.client_address);
    
    *accept_connection = 0;
    if((client.socket = accept(server_socket, (struct sockaddr *) &client.client_address, (socklen_t*) &addrlen)) < 0){
        if (errno != EWOULDBLOCK)
        {
            error("Could not accept the client.");
        }
    }
    else
    {
        *accept_connection = 1;
        printf("Successfully connected.\n");  // delete if not needed.
    }

    return client;
}

void closeServer()
{
    resetFlags();
    close(server_socket);
}

void setToNonBlocking()
{
    fcntl (server_socket, F_SETFL, server_socket_opt | O_NONBLOCK);
}

void resetFlags()
{
    fcntl (server_socket, F_SETFL, server_socket_opt);
}