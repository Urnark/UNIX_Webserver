#include "../include/response.h"

char* get_server_time(char* time_string)
{
    time_string = malloc(256);
    time_t rawtime = time(NULL);
    struct tm *ptm = gmtime(&rawtime);
    strftime(time_string, 256, "%a, %d %b %Y %T %Z", ptm);
    return time_string;
}

MyFile* define_content(Request_t* request)
{
    MyFile* file = malloc(sizeof(MyFile));
    struct stat sb;
    stat(request->path, &sb);
    FILE * f = fopen(request->path, "rb");
 
    if (f)
    {
        file->content = malloc(sb.st_size + 1);
        if (file->content)
        {
            fread(file->content, 1, sb.st_size, f);
            file->content[sb.st_size] = '\0';
        }
        fclose(f);
    }
    file->length = sb.st_size;
    printf("New: %d, Old: %d\n", file->length, strlen(file->content));
    //printf("filebuffer: %s\n", buffer);
    return file;
}

GET_Response* write_head(Request_t* request, Client *client)
{
    GET_Response* ret = malloc(sizeof(GET_Response));
    ret->content = malloc(4024);

    char* server_date = get_server_time(server_date);
    char* servername = read_config_file("SERVER_NAME=");
    char* content_type;
    MyFile* file = NULL;
    char code_notice[22];

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
                file = define_content(request);
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
        sprintf(ret->content, "HTTP/1.1 %d %s\nDate: %s\nServer: %s\nConnection: %s\nContent-Type: %s\nContent-Length: %d\nClient-Peer: %d\n\n",
            request->response_code, code_notice, server_date, servername, request->headers.connection, content_type, file->length, clientip);
        
        ret->header_length = strlen(ret->content);
        ret->content = realloc(ret->content, ret->header_length + file->length);

        memcpy(ret->content + strlen(ret->content) - 2, file->content, file->length);
        ret->length_body = file->length;
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
        sprintf(ret->content, "HTTP/1.1 %d %s\nDate: %s\nServer: %s\nConnection: %s\nContent-Type: %s\nContent-Length: %d\nClient-Peer: %d\n\n",
            request->response_code, code_notice, server_date, servername, request->headers.connection, content_type, file->length, clientip);
        ret->header_length = strlen(ret->content);
        ret->length_body = 0;
    }

    free(server_date);
    free(file->content);
    free(file);
    return ret;
}

int send_response(Request_t* request, Client *client)
{
    if(request->http_version == HTTP_1_0 || request->http_version == HTTP_1_1 || request->http_version == HTTP_none)
    {
        printf("Response\n");
        GET_Response* response = write_head(request, client);
        if (send(client->socket, response, response->header_length + response->length_body, 0) == -1)
        {
            fprintf(stderr, "ERROR: Can not send response to the client.\n");
        }
        printf("%s\n", response);
        free(response->content);
        free(response);
    }
    else if(request->http_version == HTTP_0_9)
    {
        MyFile* file = define_content(request); 
        size_t size = strlen(file->content);
        if (send(client->socket, file->content, file->length, 0) == -1)
        {
            fprintf(stderr, "ERROR: Can not send response to the client.\n");
        }
        printf("%s\n", file->content);
        free(file->content);
        free(file);
    }

}