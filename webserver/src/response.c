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
        file->file_content = malloc(sb.st_size + 1);
        if (file->file_content)
        {
            fread(file->file_content, 1, sb.st_size, f);
            file->file_content[sb.st_size] = '\0';
        }
        fclose(f);
    }
    file->length = sb.st_size;
    //printf("New: %d, Old: %ld\n", file->length, strlen(file->file_content));
    //printf("filebuffer: %s\n", file->file_content);
    return file;
}

int define_content_size(HTTP_HEAD response_head, Request_t* request)
{
        int size;
        FILE * f = fopen(request->path, "rb");
        int pointer = ftell(f);
        fseek(f, 0L, SEEK_END);
        size = ftell(f);
        fseek(f, pointer, SEEK_SET);
        return size;
}

int send_response(Client *client, char *response, int response_size)
{
    //int content_size = strlen(content);
    //printf("content_size: %d\n", content_size);
    if (send(client->socket, response, response_size, 0) == -1)
        {
            fprintf(stderr, "ERROR: Can not send response to the client.\n");
        }
    //printf("%s\n", content);

    free(response);
}

int build_response(HTTP_HEAD response_head, Request_t* request, Client *client, int head_true, int content_true)
{
    if(head_true == 0 && content_true == 0)
    {
        response_head.content_size = define_content_size(response_head ,request);
        MyFile* file = define_content(request);

        char *content = malloc(response_head.content_size + 256);
        printf("size: %d\n", response_head.content_size);
        printf("size: %d\n", response_head.content_size+256);
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
            content[headers_size + file->length-1]='\0';
        }
        send_response(client, content, headers_size + file->length);

        free(file->file_content);
        free(file);
        free(response_head.server_time);
    }
    else if (head_true == 0 && content_true == 1)
    {
        response_head.content_size = 0;

        char *content = malloc(response_head.content_size + 256);

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
        send_response(client, content, headers_size);

        free(response_head.server_time);
    }
    else if (head_true == 1 && content_true == 0)
    {
        response_head.content_size = define_content_size(response_head, request);

        char *content = malloc(response_head.content_size);
        MyFile* file = define_content(request);
        memcpy(content,file->file_content,file->length);
        send_response(client, content, response_head.content_size);

        free(file->file_content);
        free(file);
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
        response_head.server_time = get_server_time(response_head.server_time);
        //response_head.name = read_config_file("SERVER_NAME=");
        
        ServerConfig sc;
        read_config_file("SERVER_NAME=", sc);
        strcpy(response_head.name,sc.config_data);
        printf("head: %s",response_head.name);
        
        //response_head.server_version = read_config_file("SERVER_VERSION=");

        read_config_file("SERVER_VERSION=", sc);
        strcpy(response_head.server_version,sc.config_data);
        printf("version: %s",response_head.server_version);

        socklen_t addr_size = sizeof(client->client_address);
        response_head.client_ip = inet_ntoa(client->client_address.sin_addr);
        response_head.content_type = "text/html";
        response_head.connection_type=request->headers.connection;

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
    }
}
