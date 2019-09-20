#include "../include/request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
                request_type.simple = 1;
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
    request_type.simple = 1;
    request_type.type = RT_NONE;
    return request_type;
}

Request_t _process_request(char* request)
{
    Request_t request_ret;
    request_ret.simple = 0;
    request_ret.type = RT_NONE;

    char* pch;
    pch = strtok(request, " ");

    // Remove the first token from the request.
    char uri[PATH_MAX];
    strcat(uri, "../www");
    int protocol_version = 0;
    int count = 0;
    char first[7] = {'\0'};
    for (int i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            if (pch != NULL)
            {
                strcat(first, pch);
                if (strcmp(pch, "GET") == 0)
                {
                    request_ret.type = RT_GET;
                }
                else if (strcmp(pch, "HEAD") == 0)
                {
                    request_ret.type = RT_HEAD;
                }
                count++;
            }
            break;
        
        case 1:
            if (pch != NULL)
            {
                strcat(uri, pch);
                count++;
            }
            break;
        case 2:
            // Check if HTTP/1.0 was used or not
            if (pch != NULL)
            {
                if (strncmp(pch, "HTTP/1.0", 8) == 0)
                {
                    protocol_version = 1;
                }
                else if (strncmp(pch, "HTTP/0.9", 8) == 0)
                {
                    request_ret.simple = 1;
                    uri[strlen(uri) - 2] = '\0';
                }
                else if (strncmp(pch, "HTTP/1.1", 8) == 0)
                {
                    /*printf("501 Not Implemented\n");
                    request_ret.response_code = 501;
                    return request_ret;*/
                }
                else
                {
                    printf("400 Bad Request\n");
                    request_ret.response_code = 400;
                    return request_ret;
                }
                count++;
            }
            break;
        case 3:
            if (pch != NULL)
            {
                /*printf("400 Bad Request\n");
                request_ret.response_code = 400;
                return request_ret;*/
            }
            break;
        }
        pch = strtok(NULL, " ");
    }
    if (count < 2)
    {
        printf("400 Bad Request\n");
        request_ret.response_code = 400;
        return request_ret;
    }
    else if (count == 2)
    {
        if (request_ret.type == RT_GET && protocol_version == 0)
        {
            request_ret.simple = 1;
            uri[strlen(uri) - 2] = '\0';
        }
    }

    // Check if other HTTP 1.0 methods where requested
    if (request_ret.type == RT_NONE)
    {
        char methods[][7] = {"OPTIONS", "POST", "PUT", "DELETE", "TRACE", "CONNECT"};
        for (int i = 0; i < 6; i++)
        {
            if (strcmp(methods[i], first) == 0)
            {
                printf("501 Not Implemented\n");
                request_ret.response_code = 501;
                return request_ret;
            }
        }
        printf("400 Bad Request\n");
        request_ret.response_code = 400;
        return request_ret;
    }

    // Check uri
    char path[PATH_MAX];
    char *real_uri = realpath(uri, path);
    printf("uri: %s, len: %zu\n", uri, strlen(uri));
    if (real_uri) {
        if (strlen(path) > strlen(path_www_folder))
        {
            if (strncmp(path_www_folder, path, strlen(path_www_folder)) != 0)
            {
                printf("403 Forbidden\n");
                request_ret.response_code = 403;
                return request_ret;
            }
            else
            {
                printf("200 OK\n");
                printf("Request: %s %s %s\n", 
                    (request_ret.type == RT_GET? "GET": (request_ret.type == RT_HEAD? "HEAD": "NONE")), 
                    real_uri, 
                    (protocol_version? "HTTP/1.0": (request_ret.simple? "HTTP/0.9":"NONE")));
                strcpy(request_ret.path, real_uri);
                request_ret.response_code = 200;
                return request_ret;
            }
        }
        else
        {
            printf("403 Forbidden\n");
            request_ret.response_code = 403;
            return request_ret;
        }
    } else {
        printf("404 Not Found\n");
        request_ret.response_code = 404;
        return request_ret;
    }
}