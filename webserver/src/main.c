#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/threadManager.h"
#include "../include/socket.h"
#include "../include/request.h"
#include "../include/response.h"
#include "../include/logging.h"
#include "../include/read_config.h"

#define START_NUM_THREADS 5

typedef struct thread_args
{
	int id;
	Client client;
} Thread_args;

void *client_thread(void *args)
{
	int thread_id = ((Thread_args *)args)->id;
	Client *client = &((Thread_args *)args)->client;

	printf("\n%d: Start Connection!\n", ((Thread_args *)args)->id);

	Request_t request = request_received(client);
	gather_response_information(&request, client);
	free_headers(&request.headers);

	//close(client->socket);
	shutdown(client->socket, SHUT_WR);
	thread_manager_exit_thread(args);
}

int shoudStopRunning(int running)
{
	// For checking if the server should be terminated.
	int retval;
	fd_set read_fds;
	struct timeval tv;

	// Watch stdin(fd 0) for input.
	FD_ZERO(&read_fds);
	FD_SET(0, &read_fds);
	// Block for 1 second
	tv.tv_sec = 0;
	tv.tv_usec = 5000;

	retval = select(1, &read_fds, NULL, NULL, &tv);
	if (retval != 0)
	{
		printf("Stop Running\n");
		// Flush input stream
		char *buffer[512];
		read(0, buffer, sizeof(buffer));
		return 0;
	}
	return 1;
}

void start_server(int port, int log, int deamon, int setting)
{
	logging_log_to_file(log);

	// Initilize the server
	set_ip_type(0);
	set_port(port);
	set_protocol(0);
	createSocket(10);
	setToNonBlocking();

	thread_manager_init_threads(START_NUM_THREADS);

	request_init();

	int running = 1;
	while (shoudStopRunning(running))
	{
		int accept_connection;
		Client client = connectToClient(&accept_connection);
		if (accept_connection == 1)
		{
			Thread_args *args = malloc(sizeof(Thread_args));
			args->client = client;
			int rc = thread_manager_new_thread(client_thread, (void *)args);
			if (rc)
			{
				fprintf(stderr, "ERROR: pthread_create() returned %d\n", rc);
				exit(-1);
			}
		}
	}
	request_stop_reciving_data = 1;

	thread_manager_terminate_threads();
	closeServer();
}

int read_int_from_file(char *config_line)
{
	ServerConfig sc;
	read_config_file(config_line, &sc);
	char *configuration = malloc(strlen(sc.config_data) + 1);
	strcpy(configuration, sc.config_data);
	free(sc.config_data);
	char *end;
	long int result = strtol(configuration, &end, 10);
	if (end == configuration)
	{
		printf("Could not read %s from \"lab2-config\" file.\n", configuration);
		free(configuration);
		exit(-1);
	}
	else
	{
		free(configuration);
		return result;
	}
}

int main(int argc, char const *argv[])
{
	int given_port;
	int given_port_true = 1;
	int deamon;
	int deamon_true = 1;
	int log;
	int log_true = 1;
	int setting;
	int setting_true = 1;

	if (argc != 1)
	{
		int number_arguments = 0;
		while (argv[number_arguments] != NULL)
		{
			if (strcmp(argv[number_arguments], "-h") == 0)
			{
				printf("Usage: server [OPTION]...\n\n");
				printf("%s %-20s %-20s","", "-h,", "dislay help.\n");
				printf("%s %-20s %-20s","", "-p [0-65535],", "overwrites the default port used by the server on initialization.\n");
				printf("%s %-20s %-20s","", "-d,", "starts the server as deamon.\n");
				printf("%s %-20s %-20s","", "-l,", "creates a logfile to write in.\n");
				printf("%s %-20s %-20s","", "-s [thread | fork ],", "sets the requesthandling method of the server.\n");
				printf("%s %-20s %-20s","", "-r,", "repaires the configuration file and restores the default values.\n");
				return 0;
			}
			else if (strcmp(argv[number_arguments], "-p") == 0)
			{
				if (argv[number_arguments + 1] != NULL)
				{
					char *end;
					long int result = strtol(argv[number_arguments + 1], &end, 10);
					if (end == argv[number_arguments + 1])
					{
						printf("Wrong input. Try -h for help and command information.\n");
						exit(-1);
						return 3;
					}
					else
					{
						given_port = result;
						given_port_true = 0;
						number_arguments++;
					}
				}
				else
				{
					printf("Wrong input. Try -h for help and command information.\n");
					exit(-1);
					return 3;
				}
			}
			else if (strcmp(argv[number_arguments], "-d") == 0)
			{
				deamon = 1;
				deamon_true = 0;
			}
			else if (strcmp(argv[number_arguments], "-l") == 0)
			{
				log = 1;
				log_true = 0;
			}
			else if (strcmp(argv[number_arguments], "-s") == 0)
			{
				if (argv[number_arguments + 1] != NULL)
				{
					if (strcmp(argv[number_arguments + 1], "thread") == 0)
					{
						setting = 0;
						setting_true = 0;
						number_arguments++;
					}
					else if (strcmp(argv[number_arguments + 1], "fork") == 0)
					{
						setting = 1;
						setting_true = 0;
						number_arguments++;
					}
					else
					{
						printf("Wrong input. Try -h for help and command information.\n");
						exit(-1);
						return 3;
					}
				}
				else
				{
					printf("Wrong input. Try -h for help and command information.\n");
					exit(-1);
					return 3;
				}
			}
			else if (strcmp(argv[number_arguments], "-r") == 0)
			{
				if (repair_config_file())
				{
					exit(0);
					return 0;
				}
				else
				{
					exit(-1);
					return 3;
				}
			}
			number_arguments++;
		}

		if (given_port_true == 1)
		{
			given_port = read_int_from_file("SERVER_PORT=");
		}
		if (deamon_true == 1)
		{
			deamon = read_int_from_file("SERVER_AS_DEAMON=");
		}
		if (log_true == 1)
		{
			log = read_int_from_file("SERVER_LOG=");
		}
		if (setting_true == 1)
		{
			setting = read_int_from_file("SERVER_SETTING=");
		}

		start_server(given_port, log, deamon, setting);
	}
	else
	{
		//default start
		given_port = read_int_from_file("SERVER_PORT=");
		deamon = read_int_from_file("SERVER_AS_DEAMON=");
		log = read_int_from_file("SERVER_LOG=");
		setting = read_int_from_file("SERVER_SETTING=");

		start_server(given_port, log, deamon, setting);
	}
	return 0;
}
