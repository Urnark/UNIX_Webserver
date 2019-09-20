#include "../include/response.h"

int content;

int define_content()
{
    //define content to send? index.html?
}

char get_server_time()
{
    char time_string[256] = {0};
    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);
    strftime(time_string, 256, "%a, %d %b %Y %T %Z", ptm);
    return time_string;
}

int send_response(int code, Client *client)
{
    int code_notice;
    switch (code)
    {
    case 200:
        code_notice = "OK";
        break;
    case 400:
        code_notice = "Bad Request";
        break;
    case 403:
        code_notice = "Forbidden";
        break;
    case 404:
        code_notice = "Not Found";
        break;
    case 500:
        code_notice = "Internal Server Error";
        break;
    case 501:
        code_notice = "Not Implemented";
        break;
    }
    int connection = "connected";
    char server_date = get_server_time;
    int servername = "Card Server";
    int content_type = "text/html; charset=UTF-8";
    int content_length = sizeof(content);
    int client_date = "?";         //get out of Client
    int client_peer = "!";         //get out of Client
    int client_response_num = "x"; //count request-responses

    char response[256];
    sprintf(response, "HTTP/1.1 %d %d\nConnection: %d\nDate: %d\nServer: %d\nContent-Type: %d\nContent-Length: %d\nClient-Date: %d\nClient-Peer: %d\nClient-Response-Num: %d\n %d", code, code_notice, connection, server_date, servername, content_type, content_length, client_date, client_peer, client_response_num, content);

    if (send(client->socket, response, 147, 0) == -1)
    {
        fprintf(stderr, "ERROR: Can not send response to the client.\n");
    }
}