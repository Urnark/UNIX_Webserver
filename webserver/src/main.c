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
				printf("%s %-20s %-20s","", "-j,", "run server with chroot, jail.\n");
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
			else if (strcmp(argv[number_arguments], "-j") == 0)
			{
				#if MY_DEBUG
				use_jail = 0;
				fprintf(stderr, "ERROR: jail is not to be used if compiled with DEBUG.\n");
				printf("Running server in default mode instead of DEBUG mode.\n");
				#else
				use_jail = 1;
				#endif // DEBUG
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

		start_server(given_port, log, deamon, setting, use_jail);
	}
	else
	{
		//default start
		given_port = read_int_from_file("SERVER_PORT=");
		deamon = read_int_from_file("SERVER_AS_DEAMON=");
		log = read_int_from_file("SERVER_LOG=");
		setting = read_int_from_file("SERVER_SETTING=");

		start_server(given_port, log, deamon, setting, use_jail);
	}
	return 0;
}
