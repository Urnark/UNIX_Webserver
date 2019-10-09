// Need to be defined for chroot to work
//#define _POSIX_C_SOURCE 199309L
//#define _XOPEN_SOURCE 500
//#define _BSD_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/read_config.h"
#include "../include/server.h"

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

void print_usage()
{
	printf("Usage: server [-dlrj] [-p port_number] [-s thread | -s fork]\n\n");	
	printf("%s %-20s %-20s","", "-h,", "dislay help.\n");
	printf("%s %-20s %-20s","", "-p [0-65535],", "overwrites the default port used by the server on initialization.\n");
	printf("%s %-20s %-20s","", "-d,", "starts the server as deamon.\n");
	printf("%s %-20s %-20s","", "-l,", "creates a logfile to write in.\n");
	printf("%s %-20s %-20s","", "-s [thread | fork ],", "sets the requesthandling method of the server.\n");
	printf("%s %-20s %-20s","", "-r,", "repaires the configuration file and restores the default values.\n");
	printf("%s %-20s %-20s","", "-j,", "run server with chroot, jail.\n");
}

void server_exit(int exit_code)
{
	free_configurations();
	exit(exit_code);
}

int main(int argc, char const *argv[])
{
	// Open lab2-config before using chroot
	init_configurations();

	int given_port;
	int given_port_true = 1;
	int deamon;
	int deamon_true = 1;
	int log;
	int log_true = 1;
	int setting;
	int setting_true = 1;
	int use_jail = 0;
	int jail_true = 1;

	if (argc != 1)
	{
		int number_arguments = 0;
		while (argv[number_arguments] != NULL)
		{
			if (strcmp(argv[number_arguments], "-h") == 0)
			{
				print_usage();
				server_exit(0);
			}
			else if (strcmp(argv[number_arguments], "-p") == 0)
			{
				if (argv[number_arguments + 1] != NULL)
				{
					char *end;
					long int result = strtol(argv[number_arguments + 1], &end, 10);
					if (end == argv[number_arguments + 1])
					{
						printf("Wrong input.\n");
						print_usage();
						server_exit(3);
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
					printf("Wrong input.\n");
					print_usage();
					server_exit(3);
				}
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
						printf("Wrong input.\n");
						print_usage();
						server_exit(3);
					}
				}
				else
				{
					printf("Wrong input.\n");
					print_usage();
					server_exit(3);
				}
			}
			else if (argv[number_arguments][0] == '-')
			{
				for (int i = 1; i < strlen(argv[number_arguments]); i++)
				{
					switch (argv[number_arguments][i])
					{
					case 'd':
						deamon = 1;
						deamon_true = 0;
						break;
					case 'l':
						log = 1;
						log_true = 0;
						break;
					case 'r':
						if (repair_config_file())
						{
							printf("Repaired configuration file.\n");
							server_exit(0);
						}
						else
						{
							fprintf(stderr, "ERROR: Repaired configuration file.\n");
							server_exit(-1);
						}
						break;
					case 'j':
						#if MY_DEBUG
						use_jail = 0;
						fprintf(stderr, "ERROR: jail is not to be used if compiled with DEBUG.\n");
						printf("Running server in default mode instead of DEBUG mode.\n");
						#else
						use_jail = 1;
						#endif // DEBUG
						
						jail_true = 0;
						break;
					default:
						printf("Input [%s] not supported.\n", &argv[number_arguments][i]);
						print_usage();
						server_exit(3);
						break;
					}
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
		if (jail_true == 1)
		{
			use_jail = read_int_from_file("SERVER_JAIL=");
		}

		start_server(server_configurations.document_root_path, given_port, log, deamon, setting, use_jail);
	}
	else
	{
		//default start
		given_port = read_int_from_file("SERVER_PORT=");
		deamon = read_int_from_file("SERVER_AS_DEAMON=");
		log = read_int_from_file("SERVER_LOG=");
		setting = read_int_from_file("SERVER_SETTING=");
		use_jail = read_int_from_file("SERVER_JAIL=");

		start_server(server_configurations.document_root_path, given_port, log, deamon, setting, use_jail);
	}
	server_exit(0);
}
