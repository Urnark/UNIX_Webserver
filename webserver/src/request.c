#include "../include/request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

/**
 * Internal function! Sets the path to the rigth folder
 * 
 * document_root_path: The server's documents root path, ex: absolute path to the www folder
 * use_jail: If jail should be used or not
 * */
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
    }
}

/**
 * Free the allocated memory of the headers
 * */
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

/**
 * Internal function! Sets all the fields of the header struct to NULL
 * */
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

/**
 * Internal function! Set the header as the second field of the current_line.
 * 
 * header: The current header of the request
 * current_line: The current line in the request
 * comp_str: The field that is to be removed from the current_line
 * 
 * Returns: If it succeeded or not
 * */
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

/**
 * Internal function! Set the rigth field in the header struct depending on the current_line.
 * 
 * headers: A instance of the header struct
 * current_line: The current line in the request
 * 
 * Returns: The request type, GET, HEAD or NONE. This is only used if the current_line have the request method.
 * */
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

/**
 * Internal function! Check if the method of the request is not implemented or if it is some thing else.
 * 
 * request: A instance of the request struct
 * rt: The method of the request, if not RT_NONE then it skips the check
 * first_word: The first field of the request line
 * 
 * Returns: 1 if the request is RT_NONE else 0
 * */
int _check_method(Request_t* request, Request_type rt, char* first_word)
{
    // Check if method is GET or HEAD. If not check if it should answer with 501 or 400.
    request->type = rt;
    if (rt == RT_NONE)
    {
        char methods[][7] = {"OPTIONS", "POST", "PUT", "DELETE", "TRACE", "CONNECT"};
        int i;
        for (i = 0; i < 6; i++)
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

/**
 * Internal function! Check what HTTP version the reqeust from the client is.
 * 
 * request: Instance of the request struct
 * method: The rest of the reqeust line. Expect a char* with the second filed as the http version
 * 
 * Returns: 1 if an error occurs else 0
 * */
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

/**
 * Internal function! Check if the URI is valid or not.
 * 
 * request: Instance of the request struct
 * method: The URI
 * use_jail: If jail is used or not
 * 
 * Returns: 1 if an error occurs else 0
 * */
int _check_uri(Request_t* request, char* method, int use_jail)
{
    // Only used if not in jail
    char* relative_path_to_www = NULL;

    char uri[PATH_MAX];
    if (!use_jail)
    {
        char* ptr = strrchr(path_www_folder, '/');
        relative_path_to_www = malloc(strlen("..") + strlen(ptr) + 1);
        strcpy(relative_path_to_www, "..");
        strcat(relative_path_to_www, ptr);

        strcat(uri, relative_path_to_www);
    }
    size_t n = strlen(method);
    // If a space if at the end of the URI, recalculate the length of the URI string
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
    // If simple request, make sure that the uri has not a space at the end of it
    if (request->http_version == HTTP_0_9)
        uri[strlen(uri) - 1] = '\0';

    // Check if the uri is missing
    if (use_jail? uri[0] == '\0' : strcmp(uri, relative_path_to_www) == 0)
    {
        printf("400 Bad Request, missing URI\n");
        request->response_code = 400;
        return 1;
    }

    if (relative_path_to_www)
    {
        free(relative_path_to_www);
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
    char *real_uri = uri;
    if (!use_jail)
    {
        real_uri = realpath(uri, path);
    }
    
    if (real_uri) {
        if (strlen(real_uri) >= strlen(path_www_folder))
        {
            if (strncmp(path_www_folder, real_uri, strlen(path_www_folder)) != 0)
            {
                printf("403 Forbidden1\n");
                request->response_code = 403;
                return 1;
            }
            else
            {
                // If the uri want the error.html file respond with a status code of 403
                char temp[PATH_MAX];
                strcpy(temp, path_www_folder);
                strcat(temp, "/error.html");
                if (strcmp(temp, real_uri) == 0)
                {
                    printf("403 Forbidden2\n");
                    request->response_code = 403;
                    return 1;
                }

                // If only a / is used as a URI add the index.html file as a default request
                if (strcmp(path_www_folder, real_uri) == 0)
                {
                    strcat(real_uri, "/index.html");
                    strcpy(request->path, real_uri);
                }
                else
                {
                    strcpy(request->path, real_uri);
                }
                // If jail remove the / at the start of the URI
                if (use_jail)
                {
                    int i;
                    for (i = 0; i < strlen(request->path) - 1; i++)
                    {
                        request->path[i] = request->path[i + 1];
                    }
                    request->path[strlen(request->path) - 1] = '\0';
                }
                return 0;
            }
        }
        else
        {
            printf("403 Forbidden3\n");
            request->response_code = 403;
            return 1;
        }
    } else {
        printf("404 Not Found\n");
        request->response_code = 404;
        return 1;
    }
}

/**
 * Internal function! Add The first field that was removed from the request when checking the request method.
 * 
 * first_word: The method that should be added to the request line
 * request_ret: Instance of the request struct
 * */
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

/**
 * Internal function! Check if the request is too long.
 * 
 * request: The requets that is to be checked
 * 
 * Return 1 if it is too long else 0
 * */
int _string_length_check(char* request)
{
    int i;
    for (i = 0; i < PATH_MAX; i++)
    {
        if (request[i] == '\0')
        {
            return 0;
        }
    }
    return 1;
}

/**
 * Internal function! Process the request.
 * 
 * request: The request that the client sent to the server
 * use_jail: If jail is used or not
 * 
 * Returns: A filled request struct
 * */
Request_t _process_request(char* request, int use_jail)
{
    Request_t request_ret;
    request_ret.http_version = HTTP_1_0;
    request_ret.type = RT_NONE;
    request_ret.response_code = 200;
    _init_headers(&request_ret.headers);

    if (_string_length_check(request))
    {
        printf("400 Bad Request\n");
        request_ret.response_code = 400;
        return request_ret;
    }

    // If the request is smaller that 3, the length of the smallest method (GET), the request is a bad request
    if (strlen(request) < 3)
    {
        printf("400 Bad Request\n");
        request_ret.response_code = 400;
        return request_ret;
    }

    // Divied the request into lines, and check line by line.
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

/**
 * Initialize the request.
 * 
 * document_root_path: The server's documents root path, ex: absolute path to the www folder
 * use_jail: If jail should be used or not
 * */
void request_init(char* document_root_path, int use_jail)
{
    request_stop_reciving_data = 0;
    _set_path_to_www_folder(document_root_path, use_jail);
}

/**
 * Recive a request from the cilent and process it to be used in the server.
 * 
 * client: The client that the request is expected to come from
 * use_jail: If jail should be used or not
 * 
 * Returns: A filled request struct
 * */
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