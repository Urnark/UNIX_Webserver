#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

int logging_file;
FILE* logging_fd;
FILE* logging_fd_err;

/**
 * Close the logging
 * */
void logging_close();

/**
 * Tell the logging to get the paths to the files that is should use
 * */
void logging_get_path();

/**
 * Open the two files or the syslog
 * */
void logging_open(int to_file);

/**
 * Internal function! Loggs a request to the file descriptor
 * 
 * fd: The file descriptor that is to be written to
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * referer: Referer, the page that the request comes from
 * user_agent: Which browser the request was sent from
 * */
void _logging_log_f(FILE* fd, char* ip, char* userid, char* time, char* request, int response_code, char* size_in_bytes, char* referer, char* user_agent);

/**
 * Internal function! Loggs a request to either the syslog or the files and specify the priority of the log.
 * 
 * pri: Priority of the log, LOG_ERR or LOG_INFO
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * referer: Referer, the page that the request comes from
 * user_agent: Which browser the request was sent from
 * */
void _logging_log(int pri, char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);

/**
 * Logs a request with priority of LOG_INFO with the Combined Log Format.
 * 
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * referer: Referer, the page that the request comes from
 * user_agent: Which browser the request was sent from
 * */
void logging_log(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes);

/**
 * Logs a request with priority of LOG_ERR with the Combined Log Format.
 * 
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * referer: Referer, the page that the request comes from
 * user_agent: Which browser the request was sent from
 * */
void logging_log_err(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes);

/**
 * Logs a request with priority of LOG_INFO with the Common Log Format.
 * 
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * */
void logging_log_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);

/**
 * Logs a request with priority of LOG_ERR with the Common Log Format.
 * 
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * */
void logging_log_err_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);

#endif