#include "../include/response.h"
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
    struct tm *ptm = localtime(&rawtime);
    strftime(time_string, 256, "%a, %d %b %Y %T %Z", ptm);
    return time_string;
}

int send_response(Request_t* request, Client *client)
{
    printf("Response\n");
    char code_notice[22];
    switch (request->response_code)
    {
    case 200:
        strcpy(code_notice, "OK");
        break;
    case 400:
        strcpy(code_notice, "Bad Request");
        break;
    case 403:
        strcpy(code_notice, "Forbidden");
        break;
    case 404:
        strcpy(code_notice, "Not Found");
        break;
    case 500:
        strcpy(code_notice, "Internal Server Error");
        break;
    case 501:
        strcpy(code_notice, "Not Implemented");
        break;
    }
    char connection[] = "connected";
    char* server_date = get_server_time(server_date);
    char servername[] = "Card Server";
    char content_type[] = "text/html; charset=UTF-8";
    char* content = define_content(request);
    int content_length = strlen(content);
    char client_date[] = "?";         //get out of Client
    char client_peer[] = "!";         //get out of Client
    char client_response_num[] = "x"; //count request-responses

    size_t size = strlen(connection) + strlen(server_date) + strlen(servername) + strlen(content_type) + strlen(content) + 
        sizeof(content_length) + strlen(client_date) + strlen(client_peer) + strlen(client_response_num) + 149;
    char* response = malloc(size);
    
    sprintf(response, "HTTP/1.1 %d %s\nConnection: %s\nDate: %s\nServer: %s\nContent-Type: %s\nContent-Length: %d\nClient-Date: %s\nClient-Peer: %s\nClient-Response-Num: %s\n\n %s\n", 
        request->response_code, code_notice, connection, server_date, servername, content_type, content_length, client_date, client_peer, client_response_num, content);
    
    
    if (send(client->socket, response, size, 0) == -1)
    {
        fprintf(stderr, "ERROR: Can not send response to the client.\n");
    }

    printf("%s\n", response);

    free(response);
    free(content);
    free(server_date);
}