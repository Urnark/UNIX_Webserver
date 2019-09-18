#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char * ip_type, protocol_type;
struct sockaddr_in server_address, client_address;
int portnumber;
int server_socket;

void error(char * message)
{
    perror(message);
    exit(1);
}

//Sets the protocoll to either TCP (SOCK_STREAM) or UDP(SOCK_DGRAM)
int set_protocol (int i)
{
    if(i = 0){
        protocol_type = "SOCK_STREAM";
    }else if(i = 1){
        protocol_type = "SOCK_DGRAM";
    }else{
        error("Could not properly set the protocoltype.");
    }
    return(0);
}

//Sets the type either to IPv4(AF_INET) or IPv6(AF_INET)
int set_ip_type (int i)
{
    if(i == 0){
        ip_type = "AF_INET";
    }else if(i == 1){
        ip_type = "AF_INET6";
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
int createSocket (int queuelength, int portnumber)
{
    if((server_socket = socket(ip_type, protocol_type, 0)) < 0){
        error("Could not create the socket_descriptor.");
    }

    //if further settings neccessary use setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)?

    server_address.sin_family = protocol_type;
    server_address.sin_port = portnumber;
    server_address.sin_addr.s_addr = INADDR_ANY; //reads own IP address

    if((bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address))) < 0){
        error("Could not bind the socket.");
    }

    if((listen(server_socket, queuelength)) < 0){
        error("Server could not be set to listen.");
    }

    printf("Successfully created Server_Socket");  // delete if not needed.
    return 0;
}

//accept a client.
int connectToClient()
{
    int new_socket;

    if((new_socket = accept(server_socket, (struct sockaddr *) &client_address, sizeof(client_address))) < 0){
        error("Could not accept the client.");
    }

    printf("Successfully connected.");  // delete if not needed.
    return 0;
}