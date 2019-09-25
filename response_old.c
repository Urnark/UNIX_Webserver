#include "../include/response_old.h"
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

char* define_content(Request_t* request)
{
    char * buffer = NULL;
    long length;
    struct stat sb;
    stat(request->path, &sb);
    FILE * f = fopen(request->path, "rb");
 
    if (f)
    {
        buffer = malloc(sb.st_size + 1);
        if (buffer)
        {
            fread(buffer, 1, sb.st_size, f);
            buffer[sb.st_size] = '\0';
        }
        fclose(f);
    }

    return buffer;
}

char* get_server_time(char* time_string)
{
    time_string = malloc(256);
    time_t rawtime = time(NULL);
    struct tm *ptm = gmtime(&rawtime);
    strftime(time_string, 256, "%a, %d %b %Y %T %Z", ptm);
    return time_string;
}

int send_response(Request_t* request, Client *client)
{
    printf("Response\n");

    char* server_date = get_server_time(server_date);
    char servername[] = "Card Server";

    int counter = 0;

    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int clientip = getpeername(client->socket, (struct sockaddr *)&addr, &addr_size);

    char *ext = strrchr(request->path, '.');
    ext = ext + 1;
    char *content_type;

    if(strcmp(ext,"css") == 0){
        content_type = "text/css";
    }else if(strcmp(ext,"png") == 0){
        content_type = "image/png";
    }else{
	    content_type = "text/html";
	}

    char* content;

    char code_notice[22];
    switch (request->response_code)
    {
    case 200:
        strcpy(code_notice, "OK");
        content = define_content(request);
        break;
    case 400:
        strcpy(code_notice, "Bad Request");
        break;
    case 403:
        strcpy(code_notice, "Forbidden");
        break;
    case 404:
        strcpy(code_notice, "Not Found");
        content = "\n";
        break;
    case 500:
        strcpy(code_notice, "Internal Server Error");
        break;
    case 501:
        strcpy(code_notice, "Not Implemented");
        break;
    }
    
    int content_length = strlen(content);

    size_t size = sizeof(request->response_code) + strlen(code_notice) + strlen(request->headers.connection) + strlen(server_date) + strlen(servername) + strlen(content_type) + strlen(content) + 
        sizeof(content_length) + sizeof(counter) + sizeof(clientip) + 134;
    char* response = malloc(size);
    
    sprintf(response, "HTTP/1.1 %d %s\nConnection: %s\nDate: %s\nServer: %s\nContent-Type: %s\nContent-Length: %d\nClient-Peer: %d\nClient-Response-Num: %d\n\n %s\n", 
        request->response_code, code_notice, request->headers.connection, server_date, servername, content_type, content_length, clientip, counter, content);
    
    
    if (send(client->socket, response, size, 0) == -1)
    {
        fprintf(stderr, "ERROR: Can not send response to the client.\n");
    }

    printf("%s\n", response);

    free(response);
    free(content);
    free(server_date);
}