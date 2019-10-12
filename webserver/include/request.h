#ifndef REQUEST_H
#define REQUEST_H

#include <linux/limits.h>

#include "../include/socket.h"

#define REQUEST_TIMEOUT_SEC 60

typedef enum request_type{
    RT_GET,
    RT_HEAD,
    RT_NONE
} Request_type;

typedef struct headers_t
{
    char* method;
    char* host;
    char* user_agent;
    char* accept;
    char* accept_language;
    char* accept_encoding;
    char* connection;
    char* referer;
} Headers;

typedef enum http_version_t {
    HTTP_0_9,
    HTTP_1_0,
    HTTP_1_1,
    HTTP_none
} HTTP_version;

typedef struct request_t
{
    HTTP_version http_version;
    int response_code;
    char path[PATH_MAX];
    Request_type type;
    Headers headers;
} Request_t;

char path_www_folder[PATH_MAX];
int request_stop_reciving_data;

/**
 * Internal function! Sets the path to the rigth folder
 * 
 * document_root_path: The server's documents root path, ex: absolute path to the www folder
 * use_jail: If jail should be used or not
 * */
void _set_path_to_www_folder(char* document_root_path, int use_jail);

/**
 * Free the allocated memory of the headers
 * */
void free_headers(Headers* headers);

/**
 * Internal function! Sets all the fields of the header struct to NULL
 * */
void _init_headers(Headers* headers);

/**
 * Internal function! Set the header as the second field of the current_line.
 * 
 * header: The current header of the request
 * current_line: The current line in the request
 * comp_str: The field that is to be removed from the current_line
 * 
 * Returns: If it succeeded or not
 * */
int _set_header(char** header, char* current_line, const char* comp_str);

/**
 * Internal function! Set the rigth field in the header struct depending on the current_line.
 * 
 * headers: A instance of the header struct
 * current_line: The current line in the request
 * 
 * Returns: The request type, GET, HEAD or NONE. This is only used if the current_line have the request method.
 * */
Request_type _populate_headers(Headers* headers, char* current_line);

/**
 * Internal function! Check if the method of the request is not implemented or if it is some thing else.
 * 
 * request: A instance of the request struct
 * rt: The method of the request, if not RT_NONE then it skips the check
 * first_word: The first field of the request line
 * 
 * Returns: 1 if the request is RT_NONE else 0
 * */
int _check_method(Request_t* request, Request_type rt, char* first_word);

/**
 * Internal function! Check what HTTP version the reqeust from the client is.
 * 
 * request: Instance of the request struct
 * method: The rest of the reqeust line. Expect a char* with the second filed as the http version
 * 
 * Returns: 1 if an error occurs else 0
 * */
int _check_http_version(Request_t* request, char* method);

/**
 * Internal function! Check if the URI is valid or not.
 * 
 * request: Instance of the request struct
 * method: The URI
 * use_jail: If jail is used or not
 * 
 * Returns: 1 if an error occurs else 0
 * */
int _check_uri(Request_t* request, char* method, int use_jail);

/**
 * Internal function! Process the request.
 * 
 * request: The request that the client sent to the server
 * use_jail: If jail is used or not
 * 
 * Returns: A filled request struct
 * */
Request_t _process_request(char* request, int use_jail);

/**
 * Initialize the request.
 * 
 * document_root_path: The server's documents root path, ex: absolute path to the www folder
 * use_jail: If jail should be used or not
 * */
void request_init(char* document_root_path, int use_jail);

/**
 * Recive a request from the cilent and process it to be used in the server.
 * 
 * client: The client that the request is expected to come from
 * use_jail: If jail should be used or not
 * 
 * Returns: A filled request struct
 * */
Request_t request_received(Client* client, int use_jail);

#endif