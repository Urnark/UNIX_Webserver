#include "../include/response.h"

char* get_server_time(char* time_string)
{
    time_string = malloc(256);
    time_t rawtime = time(NULL);
    struct tm *ptm = gmtime(&rawtime);
    strftime(time_string, 256, "%a, %d %b %Y %T %Z", ptm);
    return time_string;
}

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
    printf("filebuffer: %s\n", buffer);
    return buffer;
}

char* write_head(Request_t* request, Client *client)
{
    char* head = malloc(1024);

    char* server_date = get_server_time(server_date);
    char* servername = read_config_file("SERVER_NAME=");
    char* content_type;
    char* content = malloc(1024);
    char code_notice[22];
    int content_length;

    socklen_t addr_size = sizeof(client->client_address);
    //int clientip = getpeername(client->socket, (struct sockaddr *)&client->client_address, &addr_size);
    int clientip = inet_ntoa(AF_INET, &(client->client_address.sin_addr), client->client_address, addr_size);

    if(request->type == RT_GET)
    {
        char *ext = strrchr(request->path, '.');
        ext = ext + 1;
    
        if(strcmp(ext,"css") == 0)
        {
            content_type = "text/css";
        }
        else if(strcmp(ext,"png") == 0)
        {
            content_type = "image/png";
        }
        else
        {
	        content_type = "text/html";
	    }

        switch (request->response_code)
        {
            case 200:
                strcpy(code_notice, "OK");
                content = define_content(request);
                content_length = strlen(content);
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
        sprintf(head, "HTTP/1.1 %d %s\nDate: %s\nServer: %s\nConnection: %s\nContent-Type: %s\nContent-Length: %d\nClient-Peer: %d\n\n %s\n",
            request->response_code, code_notice, server_date, servername, request->headers.connection, content_type, content_length, clientip, content);
            
    }
    else
    {
        content_type = "text/html";

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
        sprintf(head, "HTTP/1.1 %d %s\nDate: %s\nServer: %s\nConnection: %s\nContent-Type: %s\nContent-Length: %d\nClient-Peer: %d\n\n",
            request->response_code, code_notice, server_date, servername, request->headers.connection, content_type, content_length, clientip);
            
    }

    return head;

    free(server_date);
    free(head);
    free(content);
}

int send_response(Request_t* request, Client *client)
{
    if(request->http_version == HTTP_1_0 || request->http_version == HTTP_1_1 || request->http_version == HTTP_none)
    {
        printf("Response\n");
        char* response = write_head(request, client);
        size_t size = strlen(response);
        if (send(client->socket, response, size, 0) == -1)
        {
            fprintf(stderr, "ERROR: Can not send response to the client.\n");
        }
        printf("%s\n", response);
    }
    else if(request->http_version == HTTP_0_9)
    {
        char* content = define_content(request); 
        size_t size = strlen(content);
        if (send(client->socket, content, size, 0) == -1)
        {
            fprintf(stderr, "ERROR: Can not send response to the client.\n");
        }
        printf("%s\n", content);
    }

}