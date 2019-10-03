#include "../include/response.h"
#include <math.h>
#include "../include/logging.h"

char* get_server_time(char* time_string)
{
    time_string = malloc(256);
    time_t rawtime = time(NULL);
    struct tm *ptm = gmtime(&rawtime);
    strftime(time_string, 256, "%a, %d %b %Y %T %Z", ptm);
    return time_string;
}

int define_content_size(Request_t* request)
{
        int size;
        FILE * f = fopen(request->path, "rb");
        int pointer = ftell(f);
        fseek(f, 0L, SEEK_END);
        size = ftell(f);
        fseek(f, pointer, SEEK_SET);
        return size;
}

MyFile* define_content(Request_t* request)
{
    MyFile* file = malloc(sizeof(MyFile));
    FILE * f = fopen(request->path, "rb");
 
    if (f)
    {
        file->length = define_content_size(request);
        file->file_content = malloc(file->length + 1);
        if (file->file_content)
        {
            fread(file->file_content, 1, file->length, f);
            file->file_content[file->length] = '\0';
        }
        fclose(f);
    }
    return file;
}

void add_log(HTTP_HEAD* response_head, Request_t* request, int size)
{
    request->headers.method[strlen(request->headers.method) - 1] = '\0';
    logging_log(response_head->client_ip, "-", response_head->server_time, request->headers.method, request->response_code, size);
}

int send_response(Client *client, char *response, int response_size)
{
    if (send(client->socket, response, response_size, 0) == -1)
    {
        fprintf(stderr, "ERROR: Can not send response to the client.\n");
        return 0;
    }

    free(response);
    return 1;
}

int build_response(HTTP_HEAD response_head, Request_t* request, Client *client, int head_true, int content_true)
{
    if(head_true == 0 && content_true == 0)
    {
        response_head.content_size = define_content_size(request);
        MyFile* file = define_content(request);

        response_head.server_time = get_server_time(response_head.server_time);

        int header_length = strlen(response_head.http_version) + 
                            strlen(response_head.code) +
                            strlen(response_head.code_notice) +
                            strlen(response_head.server_time) +
                            strlen(response_head.name) +
                            strlen(response_head.server_version) +
                            strlen(response_head.content_type) +
                            (response_head.content_size == 0 ? 1 : (int)(log10f((float)(response_head.content_size))+1)) +
                            strlen(response_head.connection_type) +
                            strlen(response_head.client_ip) +
                            107;
        char *content = malloc(response_head.content_size + header_length);
        sprintf(content, "%s %s %s\nDate: %s\nServer: %s\nServer_version: %s\nContent-Type: %s\nContent-Length: %d\nConnection: %s\nClient-Peer: %s\n\n",
            response_head.http_version,
            response_head.code,
            response_head.code_notice,
            response_head.server_time,
            response_head.name,
            response_head.server_version,
            response_head.content_type,
            response_head.content_size,
            response_head.connection_type,
            response_head.client_ip);

        int headers_size = strlen(content);
        memcpy(content + headers_size,file->file_content,file->length);
        if (strcmp(response_head.content_type, "text/html") == 0 ||
            strcmp(response_head.content_type, "text/css") == 0)
        {
            content[headers_size + file->length]='\0';
        }

        if (send_response(client, content, headers_size + file->length))
        {
            add_log(&response_head, request, file->length);
        }

        free(file->file_content);
        free(file);
        free(response_head.server_time);
    }
    else if (head_true == 0 && content_true == 1)
    {
        response_head.content_size = 0;

        response_head.server_time = get_server_time(response_head.server_time);

        int header_length = strlen(response_head.http_version) + 
                            strlen(response_head.code) +
                            strlen(response_head.code_notice) +
                            strlen(response_head.server_time) +
                            strlen(response_head.name) +
                            strlen(response_head.server_version) +
                            strlen(response_head.content_type) +
                            1 +
                            strlen(response_head.connection_type) +
                            strlen(response_head.client_ip) +
                            107;
        char *content = malloc(response_head.content_size + header_length);

        sprintf(content, "%s %s %s\nDate: %s\nServer: %s\nServer_version: %s\nContent-Type: %s\nContent-Length: %d\nConnection: %s\nClient-Peer: %s\n\n",
            response_head.http_version,
            response_head.code,
            response_head.code_notice,
            response_head.server_time,
            response_head.name,
            response_head.server_version,
            response_head.content_type,
            response_head.content_size,
            response_head.connection_type,
            response_head.client_ip);

        int headers_size = strlen(content);

        if (send_response(client, content, headers_size))
        {
            add_log(&response_head, request, 0);
        }

        free(response_head.server_time);
    }
    else if (head_true == 1 && content_true == 0)
    {
        response_head.content_size = define_content_size(request);

        // For logging only
        response_head.server_time = get_server_time(response_head.server_time);

        char *content = malloc(response_head.content_size);
        MyFile* file = define_content(request);
        memcpy(content,file->file_content,file->length);

        if (send_response(client, content, response_head.content_size))
        {
            add_log(&response_head, request, file->length);
        }

        free(file->file_content);
        free(file);
        free(response_head.server_time);
    }
}

int gather_response_information(Request_t* request, Client *client)
{
    HTTP_HEAD response_head;

    if(request->http_version == HTTP_0_9)
    {
        build_response(response_head, request, client, 1, 0);
    }
    else if(request->http_version == HTTP_1_0 || request->http_version == HTTP_1_1 || request->http_version == HTTP_none)
    {
        if(request->http_version == HTTP_1_0){
            response_head.http_version = "HTTP/1.0";
        }else{
            response_head.http_version = "HTTP/1.1";
        }
        
        ServerConfig sc;
        read_config_file("SERVER_NAME=", &sc);
        response_head.name = malloc(strlen(sc.config_data) + 1);
        strcpy(response_head.name,sc.config_data);
        free(sc.config_data);

        read_config_file("SERVER_VERSION=", &sc);
        response_head.server_version = malloc(strlen(sc.config_data) + 1);
        strcpy(response_head.server_version,sc.config_data);
        free(sc.config_data);

        socklen_t addr_size = sizeof(client->client_address);
        response_head.client_ip = inet_ntoa(client->client_address.sin_addr);
        response_head.content_type = "text/html";
        if(request->headers.connection != NULL){
            response_head.connection_type=request->headers.connection;
        }else{
            response_head.connection_type="closed";
        }

        switch (request->response_code)
                {
                    case 200:
                        response_head.code="200";
                        response_head.code_notice="OK";
                        if(request->type == RT_GET)
                        {
                            char *ext = strrchr(request->path, '.');
                            ext = ext + 1;
                        
                            if(strcmp(ext,"css") == 0)
                            {
                                response_head.content_type="text/css";
                            }
                            else if(strcmp(ext,"png") == 0)
                            {
                                response_head.content_type="image/png";
                            }
                            
                            build_response(response_head, request, client, 0, 0);

                        }else{
                            build_response(response_head, request, client, 0, 1);
                        }
                        break;
                    case 400:
                        response_head.code="400";
                        response_head.code_notice="Bad Request";
                        build_response(response_head, request, client, 0, 1);
                        break;
                    case 403:
                        response_head.code="403";
                        response_head.code_notice="Forbidden";
                        build_response(response_head, request, client, 0, 1);
                        break;
                    case 404:
                        response_head.code="404";
                        response_head.code_notice="Not Found";
                        build_response(response_head, request, client, 0, 1);
                        break;
                    case 500:
                        response_head.code="500";
                        response_head.code_notice="Internal Server Error";
                        build_response(response_head, request, client, 0, 1);
                        break;
                    case 501:
                        response_head.code="501";
                        response_head.code_notice="Not Implemented";
                        build_response(response_head, request, client, 0, 1);
                        break;
                }
        
        free(response_head.name);
        free(response_head.server_version);
    }
}
