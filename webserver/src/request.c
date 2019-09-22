#include "../include/request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//  GET /favicon.ico HTTP/1.1
//  Host: localhost:4444
//  User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:69.0) Gecko/20100101 Firefox/69.0
//  Accept: image/webp,*/*
//  Accept-Language: en-US,en;q=0.5
//  Accept-Encoding: gzip, deflate
//  Connection: keep-alive

//  ET /index.html HTTP/1.1
//  Host: localhost:4444
//  User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:69.0) Gecko/20100101 Firefox/69.0
//  Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
//  Accept-Language: en-US,en;q=0.5
//  Accept-Encoding: gzip, deflate
//  Connection: keep-alive
//  Upgrade-Insecure-Requests: 1


//  GET /style.css HTTP/1.1
//  Host: localhost:4444
//  User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:69.0) Gecko/20100101 Firefox/69.0
//  Accept: text/css,*/*;q=0.1
//  Accept-Language: en-US,en;q=0.5
//  Accept-Encoding: gzip, deflate
//  Connection: keep-alive
//  Referer: http://localhost:4444/index.html

//    GET /images/diamond_7.png HTTP/1.1
//    Host: localhost:4444
//    User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:69.0) Gecko/20100101 Firefox/69.0
//    Accept: image/webp,*/*
//    Accept-Language: en-US,en;q=0.5
//    Accept-Encoding: gzip, deflate
//    Connection: keep-alive
//    Referer: http://localhost:4444/index.html

void _set_path_to_www_folder()
{
    // Get current working directory
    getcwd(path_www_folder, sizeof(path_www_folder));

    // Remove the last direcotry
    int len = strlen(path_www_folder);
    int index = len - 1;
    while(path_www_folder[index--] != '/'){}
    path_www_folder[index + 1] = '\0';

    // Add www to the path
    strcat(path_www_folder, "/www");
}

void _free_headers(Headers* headers)
{
    free(headers->accept);
    free(headers->accept_encoding);
    free(headers->accept_language);
    free(headers->connection);
    free(headers->method);
    free(headers->host);
    free(headers->referer);
    free(headers->user_agent);
}

int _set_header(char** header, char* current_line, const char* comp_str)
{
    if (strlen(current_line) >= strlen(comp_str))
    {
        if (strncmp(current_line, comp_str, strlen(comp_str)) == 0)
        {
            size_t step = strlen(comp_str) + 1;
            *header = (char*)malloc(strlen(current_line));
            strcpy(*header, (char*)(current_line + step));
            return 1;
        }
    }
    return 0;
}

Request_type _populate_headers(Headers* headers, char* current_line)
{
    int get = _set_header(&headers->method, current_line, "GET");
    int head = _set_header(&headers->method, current_line, "HEAD");
    _set_header(&headers->host, current_line, "Host:");
    _set_header(&headers->user_agent, current_line, "User-Agent:");
    _set_header(&headers->accept, current_line, "Accept:");
    _set_header(&headers->accept_language, current_line, "Accept-Language:");
    _set_header(&headers->accept_encoding, current_line, "Accept-Encoding:");
    _set_header(&headers->connection, current_line, "Connection:");
    _set_header(&headers->referer, current_line, "Referer:");

    return (get? RT_GET: (head? RT_HEAD: RT_NONE));
}

int _check_method(Request_t* request, Request_type rt, char* first_word)
{
    // Check if method is GET or HEAD. If not check if it should answer with 501 or 400.
    request->type = rt;
    if (rt == RT_NONE)
    {
        char methods[][7] = {"OPTIONS", "POST", "PUT", "DELETE", "TRACE", "CONNECT"};
        for (int i = 0; i < 6; i++)
        {
            if (strcmp(methods[i], first_word) == 0)
            {
                printf("501 Not Implemented\n");
                request->response_code = 501;
                return 1;
            }
        }
        printf("400 Bad Request\n");
        request->response_code = 400;
        return 1;
    }
    return 0;
}

int _check_http_version(Request_t* request, char* method)
{
    // Check what the HTTP version for the request is.
    char* p = strchr(method, ' ');
    if (p == NULL && strlen(method) >= 1)
    {
        request->http_version = HTTP_0_9;
        return 0;
    }

    if (strlen(p) != strlen(" HTTP/1.0") + 1)
    {
        printf("400 Bad Request, http version length not matching\n");
        request->response_code = 400;
        return 1; 
    }
    
    if (strncmp(p, " HTTP/0.9", 9) == 0)
    {
        request->http_version = HTTP_0_9;
        return 0;
    }
    if (strncmp(p, " HTTP/1.0", 9) == 0)
    {
        request->http_version = HTTP_1_0;
        return 0;
    }
    if (strncmp(p, " HTTP/1.1", 9) == 0)
    {
        request->http_version = HTTP_1_1;
        return 0;
    }
    
    printf("400 Bad Request, http version not real\n");
    request->response_code = 400;
    return 1;
}

int _check_uri(Request_t* request, char* method)
{
    char uri[PATH_MAX];
    strcat(uri, "../www");
    size_t n = strlen(method);
    char* p = strchr(method, ' ');
    if (p != NULL)
    {
        *p = '\0';
        n = strlen(method);
        *p = ' ';
    }
    strncat(uri, method, n);
    if (request->http_version == HTTP_0_9)
        uri[strlen(uri) - 1] = '\0';

    if (strcmp(uri, "../www") == 0)
    {
        printf("400 Bad Request, missing URI\n");
        request->response_code = 400;
        return 1;
    }

    // Check uri
    char path[PATH_MAX];
    char *real_uri = realpath(uri, path);
    if (real_uri) {
        if (strlen(path) >= strlen(path_www_folder))
        {
            if (strncmp(path_www_folder, path, strlen(path_www_folder)) != 0)
            {
                printf("403 Forbidden\n");
                request->response_code = 403;
                return 1;
            }
            else
            {
                if (strcmp(path_www_folder, path) == 0)
                {
                    strcat(path, "/index.html");
                    strcpy(request->path, real_uri);
                    return 0;
                }
                strcpy(request->path, real_uri);
                return 0;
            }
        }
        else
        {
            printf("403 Forbidden\n");
            request->response_code = 403;
            return 1;
        }
    } else {
        printf("404 Not Found\n");
        request->response_code = 404;
        return 1;
    }
}

Request_t _process_request(char* request)
{
    Request_t request_ret;
    request_ret.http_version = HTTP_1_0;
    request_ret.type = RT_NONE;
    request_ret.response_code = 200;

    Headers headers = {NULL};

    if (strlen(request) < 3)
    {
        printf("400 Bad Request\n");
        request_ret.response_code = 400;
        return request_ret;
    }

    Request_type get_head_none;
    char* current_line = request;
    while (current_line)
    {
        char* next_line = strchr(current_line, '\n');
        if (next_line)
            *next_line = '\0';
        
        if (current_line == request)
            get_head_none = _populate_headers(&headers, current_line);
        else
            _populate_headers(&headers, current_line);

        if (next_line)
            *next_line = '\n';
        current_line = (next_line? next_line + 1 : NULL);
    }

    // Checks what method is used, the uri and the HTTP version.
    // Return the right response code.
    char* first_word = strchr(request, ' ');
    if (first_word != NULL)
    {
        *first_word = '\0';
        first_word = request;
        if (_check_method(&request_ret, get_head_none, first_word))
        {
            _free_headers(&headers);
            return request_ret;
        }
        if (_check_http_version(&request_ret, headers.method))
        {
            _free_headers(&headers);
            return request_ret;
        }
        if (_check_uri(&request_ret, headers.method))
        {
            _free_headers(&headers);
            return request_ret;
        }
    }
    else
    {
        printf("400 Bad Request\n");
        request_ret.response_code = 400;
        _free_headers(&headers);
        return request_ret;
    }

    printf("Method: %s | %s | %s\nHost: %s\nUser-Agent: %s\nAccept: %s\nAccept-Language: %s\nAccept-Encoding: %s\nConnection: %s\nReferer: %s\n", 
        (get_head_none == RT_GET? "GET": "HEAD"), request_ret.path, 
        (request_ret.http_version == HTTP_0_9? "HTTP/0.9": (request_ret.http_version == HTTP_1_0? "HTTP/1.0":"HTTP/1.1")), 
        headers.host, headers.user_agent, headers.accept, headers.accept_language, headers.accept_encoding, headers.connection, headers.referer);
    
    _free_headers(&headers);
    return request_ret;
}

void request_init()
{
    request_stop_reciving_data = 0;
    _set_path_to_www_folder();
}

Request_t request_recived(Client* client)
{
    char request[PATH_MAX];
    while (!request_stop_reciving_data)
    {
        if (recv(client->socket, request, sizeof(request), 0) == -1) {
            if (errno != EWOULDBLOCK)
            {
                fprintf(stderr, "ERROR: Can not recive the request from the client.\n");
                Request_t request_type;
                request_type.response_code = 500;
                request_type.http_version = HTTP_0_9;
                request_type.type = RT_NONE;
                return request_type;
            }
	    }
        else
        {
            printf("%s\n", request);
            return _process_request(request);
        }
        
    }
    Request_t request_type;
    request_type.response_code = 503;
    request_type.http_version = HTTP_0_9;
    request_type.type = RT_NONE;
    return request_type;
}