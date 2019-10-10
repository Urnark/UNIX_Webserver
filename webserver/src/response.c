#include "../include/response.h"
#include <math.h>
#include "../include/logging.h"

MyFile* error_page;

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

void free_error_page()
{
    free(error_page->file_content);
    free(error_page);
}

void read_error_page(char* document_root_path)
{
    char error_html[] = "/error.html";

    Request_t re;
    strcpy(re.path, document_root_path);
    strcat(re.path, error_html);

    int length = define_content_size(&re);
    error_page = define_content(&re, length);
}

void fill_word(char* cpy_page, const char* word, char* to)
{
    char* ptr = strstr(cpy_page, word);
    while(ptr != NULL)
    {
        char* temp = malloc(strlen(ptr) + 1);
        strcpy(temp, ptr);
        strcpy(ptr, to);
        strcat(cpy_page, temp + strlen(word));
        free(temp);

        ptr = strstr(ptr, word);
    }
}

MyFile* fill_error_page(HTTP_HEAD* response_head)
{
    MyFile* file = malloc(sizeof(MyFile));
    file->file_content = malloc(error_page->length + 4000);
    strcpy(file->file_content, error_page->file_content);

    fill_word(file->file_content, "E_C", response_head->code);
    fill_word(file->file_content, "E_T", response_head->code_notice);
    fill_word(file->file_content, "E_S", response_head->failure_response);

    file->length = strlen(file->file_content);

    return file;
}

char* get_server_time(char* time_string)
{
    time_string = malloc(256);
    time_t rawtime = time(NULL);
    struct tm *ptm = gmtime(&rawtime);
    strftime(time_string, 256, "%a, %d %b %Y %T %Z", ptm);
    return time_string;
}

MyFile* define_content(Request_t* request, int allocated_memory)
{
    MyFile* file = malloc(sizeof(MyFile));
    FILE * f = fopen(request->path, "rb");
 
    if (f)
    {
        file->length = define_content_size(request);
        file->file_content = malloc((allocated_memory==-1?file->length + 1:allocated_memory + 1));
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
    if (request->response_code != 408 && request->response_code != 500 && request->response_code != 503)
    {
        char* str = NULL;
        if (request->headers.method != NULL)
        {
            if (strlen(request->headers.method) >= 1)
            {
                request->headers.method[strlen(request->headers.method) - 1] = '\0';
                str = request->headers.method;
            }
            else
            {
                str = malloc(strlen("-") + 1);
                strcpy(str, "-");
            }
        }
        else
        {
            str = malloc(strlen("-") + 1);
            strcpy(str, "-");
        }
    
        if (request->response_code == 200)
        {
            logging_log(response_head->client_ip, "-", response_head->server_time, str, request->response_code, size);
        }
        else
        {
            logging_log_err(response_head->client_ip, "-", response_head->server_time, str, request->response_code, size);
        }
        
        if (request->headers.method == NULL)
        {
            free(str);
        }
        else if (strlen(request->headers.method) < 1)
        {
            free(str);
        }
    }
    else
    {
        logging_log_err(response_head->client_ip, "-", response_head->server_time, "-", request->response_code, size);
    }
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
        response_head.content_size;
        MyFile* file;
        if (request->response_code == 200)
        {
            response_head.content_size = define_content_size(request);
            file = define_content(request, -1);
        }
        else
        {
            file = fill_error_page(&response_head);
            response_head.content_size = file->length;
        }

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
        MyFile* file = define_content(request, -1);
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
        if (request->response_code == 200)
        {
            build_response(response_head, request, client, 1, 0);
        }
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

        int check_content = 0;
        if (request->type == RT_HEAD)
        {
            check_content = 1;
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
                        response_head.failure_response="The server won't respond. Your client has send a malfunctionous request.";
                        build_response(response_head, request, client, 0, check_content);
                        break;
                    case 403:
                        response_head.code="403";
                        response_head.code_notice="Forbidden";
                        response_head.failure_response="You have not the permission to access this side on the server. Please contact the server administration.";
                        build_response(response_head, request, client, 0, check_content);
                        break;
                    case 404:
                        response_head.code="404";
                        response_head.code_notice="Not Found";
                        response_head.failure_response="The requested URL was not found on this server. We are sorry.";
                        build_response(response_head, request, client, 0, check_content);
                        break;
                    case 408:
                        response_head.code="408";
                        response_head.code_notice="Request Timeout";
                        response_head.failure_response="The server decided not to wait any longer for you to make a request. Good night!";
                        build_response(response_head, request, client, 0, check_content);
                        break;
                    case 500:
                        response_head.code="500";
                        response_head.code_notice="Internal Server Error";
                        response_head.failure_response="The server is not able to answer at the moment. It seems to be lost in his thoughts...";
                        build_response(response_head, request, client, 0, check_content);
                        break;
                    case 501:
                        response_head.code="501";
                        response_head.code_notice="Not Implemented";
                        response_head.failure_response="The server is not able to answer this kind of request.";
                        build_response(response_head, request, client, 0, check_content);
                        break;
                    case 503:
                        response_head.code="503";
                        response_head.code_notice="Service Unavailable";
                        response_head.failure_response="The service is unavailable.";
                        build_response(response_head, request, client, 0, check_content);
                        break;
                }
        
        free(response_head.name);
        free(response_head.server_version);
    }
}
