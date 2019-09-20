#include "../include/request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_path_to_www_folder()
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

Request_t request_recived(Client* client)
{
    Request_t request_ret;
    request_ret.simple = 0;
    request_ret.type = RT_NONE;

    char request[PATH_MAX];
	if (recv(client->socket, request, sizeof(request), 0) == -1) {
		fprintf(stderr, "ERROR: Can not recive the request from the client.\n");
	}

    char* pch;
    pch = strtok(request, " ");

    // Remove the first token from the request.
    char uri[1024];
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
                else
                {
                    printf("501 Not Implemented\n"); // Or Bad Request? Get this if request is: GET SP /index.html SP
                    request_ret.result_code = 501;
                    return request_ret;
                }
                count++;
            }
            break;
        case 3:
            if (pch != NULL)
            {
                printf("400 Bad Request\n");
                request_ret.result_code = 400;
                return request_ret;
            }
            break;
        }
        pch = strtok(NULL, " ");
    }
    if (count < 2)
    {
        printf("400 Bad Request\n");
        request_ret.result_code = 400;
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
                request_ret.result_code = 501;
                return request_ret;
            }
        }
        printf("400 Bad Request\n");
        request_ret.result_code = 400;
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
                request_ret.result_code = 403;
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
                request_ret.result_code = 200;
                return request_ret;
            }
        }
        else
        {
            printf("403 Forbidden\n");
            request_ret.result_code = 403;
            return request_ret;
        }
    } else {
        printf("404 Not Found\n");
        request_ret.result_code = 404;
        return request_ret;
    }
}