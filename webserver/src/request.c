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
    printf("Server dir: %s\n", path_www_folder);
}

int request_recived(Client* client)
{
    char request[1024];
	if (recv(client->socket, request, sizeof(request), 0) == -1) {
		fprintf(stderr, "ERROR: Can not recive the request from the client.\n");
	}

    char* pch;
    pch = strtok(request, " ");
    Request_type type = RT_NONE;

    // Remove the first token from the request.
    char uri[1024];
    strcat(uri, "../www");
    int protocol_version = 0;
    int count = 0;
    char first[7] = {'\0'};
    while (pch != NULL)
    {
        if (first[0] == '\0')
        {
            strcat(first, pch);
        }
        count++;
        if (strcmp(pch, "GET") == 0)
        {
            type = RT_GET;
        }
        else if (strcmp(pch, "HEAD") == 0)
        {
            type = RT_HEAD;
        }
        else if (strncmp(pch, "HTTP/1.0", 8) == 0)
        {
            protocol_version = 1;
        }
        else
        {
            strcat(uri, pch);
        }
        pch = strtok(NULL, " ");
    }
    if (count != 3)
    {
        printf("400 Bad Request\n");
        return 400;
    }

    // Check if other HTTP 1.0 methods where requested
    if (type == RT_NONE)
    {
        char methods[][7] = {"OPTIOND", "POST", "PUT", "DELETE", "TRACE", "CONNECT"};
        for (int i = 0; i < 6; i++)
        {
            if (strcmp(methods[i], first) == 0)
            {
                printf("501 Not Implemented\n");
                return 501;
            }
        }
        printf("400 Bad Request\n");
        return 400;
    }

    // Check if HTTP/1.0 was used or not
    if (protocol_version != 1)
    {
        printf("501 Not Implemented\n");
        return 501;
    }

    // Check uri
    char path[PATH_MAX];
    char *real_uri = realpath(uri, path);
    if (real_uri) {
        printf("This path is at %s.\n", path);
        printf("This path_www_folder is at %s.\n", path_www_folder);
        if (strlen(path) > strlen(path_www_folder))
        {
            if (strncmp(path_www_folder, path, strlen(path_www_folder)) != 0)
            {
                printf("403 Forbidden\n");
                return 403;
            }
            else
            {
                // Cech length of paths?
                // Good path
            }
        }
        else
        {
            printf("403 Forbidden\n");
            return 403;
        }
    } else {
        printf("404 Not Found\n");
        return 404;
    }

    printf("Request: %s %s %s\n", (type == RT_GET? "GET": (type == RT_HEAD? "HEAD": "NONE")), real_uri, (protocol_version? "HTTP/1.0": "NONE"));
}