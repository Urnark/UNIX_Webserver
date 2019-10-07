#include "../include/request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

void _set_path_to_www_folder(char* document_root_path, int use_jail)
{
    // Add the path to www to the path if not jail, else only /
    if (use_jail)
    {
        strcat(path_www_folder, "/");
    }
    else
    {
        strcpy(path_www_folder, document_root_path);
        strcat(path_www_folder, "/");
    }
}

void free_headers(Headers* headers)
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

void _init_headers(Headers* headers)
{
    headers->accept = NULL;
    headers->accept_encoding = NULL;
    headers->accept_language = NULL;
    headers->connection = NULL;
    headers->method = NULL;
    headers->host = NULL;
    headers->referer = NULL;
    headers->user_agent = NULL;
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
        request->http_version = HTTP_none;
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
    request->http_version = HTTP_none;
    return 1;
}

int _check_uri(Request_t* request, char* method, int use_jail)
{
    char uri[PATH_MAX];
    if (!use_jail)
    {
        strcat(uri, "../www");
    }
    size_t n = strlen(method);
    char* p = strchr(method, ' ');
    if (p != NULL)
    {
        *p = '\0';
        n = strlen(method);
        *p = ' ';
    }
    // String length check on path
    if (n >= PATH_MAX)
    {
        printf("400 Bad Request, URI faild string length check\n");
        request->response_code = 400;
        return 1;
    }
    strncat(uri, method, n);
    if (request->http_version == HTTP_0_9)
        uri[strlen(uri) - 1] = '\0';

    if (use_jail? uri[0] == '\0' : strcmp(uri, "../www") == 0)
    {
        printf("400 Bad Request, missing URI\n");
        request->response_code = 400;
        return 1;
    }

    // Remove everything after the "?" symbol in the path
    char* ptr = strchr(uri, '?');
    if (ptr != NULL)
    {
        *ptr = '\0';
    }

    printf("%s\n", uri);

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

void _add_get_head_on_method(char* first_word, Request_t* request_ret)
{
    // Do so the method variable holds ex: GET / HTTP/1.0
    int length = strlen(request_ret->headers.method);
    char* temp = malloc(length + 1);
    strcpy(temp, request_ret->headers.method);
    free(request_ret->headers.method);
    request_ret->headers.method = malloc(length + strlen(first_word) + 2);
    strcpy(request_ret->headers.method, first_word);
    strcat(request_ret->headers.method, " ");
    strcat(request_ret->headers.method, temp);
    free(temp);
}

Request_t _process_request(char* request, int use_jail)
{
    Request_t request_ret;
    request_ret.http_version = HTTP_1_0;
    request_ret.type = RT_NONE;
    request_ret.response_code = 200;
    _init_headers(&request_ret.headers);

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
            get_head_none = _populate_headers(&request_ret.headers, current_line);
        else
            _populate_headers(&request_ret.headers, current_line);

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
            _add_get_head_on_method(first_word, &request_ret);
            return request_ret;
        }
        if (_check_http_version(&request_ret, request_ret.headers.method))
        {
            _add_get_head_on_method(first_word, &request_ret);
            return request_ret;
        }
        if (_check_uri(&request_ret, request_ret.headers.method, use_jail))
        {
            _add_get_head_on_method(first_word, &request_ret);
            return request_ret;
        }
        _add_get_head_on_method(first_word, &request_ret);
    }
    else
    {
        printf("400 Bad Request\n");
        request_ret.response_code = 400;
        return request_ret;
    }

    printf("Method: %s | %s | %s\nHost: %s\nUser-Agent: %s\nAccept: %s\nAccept-Language: %s\nAccept-Encoding: %s\nConnection: %s\nReferer: %s\n", 
        (get_head_none == RT_GET? "GET": "HEAD"), request_ret.path, 
        (request_ret.http_version == HTTP_0_9? "HTTP/0.9": (request_ret.http_version == HTTP_1_0? "HTTP/1.0":(request_ret.http_version == HTTP_none? "none":"HTTP/1.1"))), 
        request_ret.headers.host, request_ret.headers.user_agent, request_ret.headers.accept, request_ret.headers.accept_language, 
        request_ret.headers.accept_encoding, request_ret.headers.connection, request_ret.headers.referer);
    
    return request_ret;
}

void request_init(char* document_root_path, int use_jail)
{
    request_stop_reciving_data = 0;
    _set_path_to_www_folder(document_root_path, use_jail);
}

Request_t request_received(Client* client, int use_jail)
{
    struct timeval start, now;
    gettimeofday(&start, NULL);

    char request[PATH_MAX];
    while (!request_stop_reciving_data)
    {
        gettimeofday(&now, NULL);
        if (now.tv_sec - start.tv_sec >= REQUEST_TIMEOUT_SEC)
        {
            printf("408 Request Timeout\n");
            Request_t request_type;
            request_type.response_code = 408;
            request_type.http_version = HTTP_none;
            request_type.type = RT_NONE;
            return request_type;
        }
        if (recv(client->socket, request, sizeof(request), 0) == -1) {
            if (errno != EWOULDBLOCK)
            {
                fprintf(stderr, "ERROR: Can not recive the request from the client.\n");
                printf("500 Internal Server Error\n");
                Request_t request_type;
                request_type.response_code = 500;
                request_type.http_version = HTTP_none;
                request_type.type = RT_NONE;
                return request_type;
            }
	    }
        else
        {
            return _process_request(request, use_jail);
        }
        
    }
    printf("503 Service Unavailable\n");
    Request_t request_type;
    request_type.response_code = 503;
    request_type.http_version = HTTP_none;
    request_type.type = RT_NONE;
    return request_type;
}