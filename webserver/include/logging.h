#ifndef LOGGING_H
#define LOGGING_H

int logging_file;

void logging_log_to_file(int to_file);
void _logging_log_f(const char* path, char* ip, char* userid, char* time, char* request, int response_code, char* size_in_bytes, char* referer, char* user_agent);
void _logging_log(int pri, char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);
void logging_log(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes);
void logging_log_err(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes);

void logging_log_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);
void logging_log_err_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent);

#endif