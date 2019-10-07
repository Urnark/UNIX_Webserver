#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

int logging_file;
FILE* logging_fd;
FILE* logging_fd_err;

void logging_close();
void logging_open(int to_file);
void _logging_log_f(FILE* fd, char* ip, char* userid, char* time, char* request, int response_code, char* size_in_bytes, char* referer, char* user_agent);
void _logging_log(int pri, char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);
void logging_log(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes);
void logging_log_err(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes);

void logging_log_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);
void logging_log_err_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);

#endif