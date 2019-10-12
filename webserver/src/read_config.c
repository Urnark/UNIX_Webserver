#include "../include/read_config.h"
#include <stdlib.h>
#include <linux/limits.h>
#include <unistd.h>

const char FILENAME[] = ".lab3-config";

/**
 * Opens the configuration-file and saves the information for later usage.
 * */
void init_configurations()
{
    server_configurations.document_root_path = NULL;
    FILE* f = fopen(FILENAME , "r");

    if(f)
    {
        int pointer = ftell(f);
        fseek(f, 0L, SEEK_END);
        int size = ftell(f);
        fseek(f, pointer, SEEK_SET);

        server_configurations.config_data = malloc(size + 1);
        if (server_configurations.config_data)
        {
            fread(server_configurations.config_data, 1, size, f);
            server_configurations.config_data[size] = '\0';
        }
        
        fclose(f);
        

        ServerConfig sc;
        read_config_file("SERVER_DOCUMENT_ROOT=", &sc);
        server_configurations.document_root_path = malloc(strlen(sc.config_data) + 1);
        strcpy(server_configurations.document_root_path, sc.config_data);
        free(sc.config_data);
    }
}

/**
 * Checks if the path to the www-folder/ source-folder for the webpages is valid.
 * */
void check_www_path()
{
    char* path = realpath(server_configurations.document_root_path, NULL);
    if (path == NULL)
    {
        fprintf(stderr, "ERROR: The path [%s] in the configuration file is invalid.\nRun with -r to repair the configuration file.\n", server_configurations.document_root_path);
        free_configurations();
        exit(-1);
    }
    else
    {
        free(path);
    }
}

/**
 * Deallocates the memory used for the data of the configuration file.
 * */
void free_configurations()
{
    free(server_configurations.config_data);
    free(server_configurations.document_root_path);
}

/**
 * Searches for a given information in the saved configuration-file.
 * 
 * data: string to compare with the information from the configuration file.
 * sc: variable with the content of the configuration file.
 * */
void read_config_file(char* data, ServerConfig* sc)
{
    char* ptr = strstr(server_configurations.config_data, data);
    int length = 0;
    char* ptr_2;
    for(ptr_2 = ptr; *ptr_2 != '\n'; ++ptr_2, length++) {};
    if (ptr != NULL)
    {
        sc->config_data = malloc(length + 1);
        strncpy(sc->config_data, ptr + strlen(data), length - strlen(data));
        sc->config_data[length - strlen(data)] ='\0';
    }
}

/**
 * Repaires the configuration-file or creates a new one, if none exists.
 * */
int repair_config_file(){
    int status;
    status = remove(FILENAME);
    //Repaires the configuration-file
    if (status == 0){
        status = create_config_file();
        if (status)
            printf("Successfully repaired \"%s\" file.\n", FILENAME);
        else
        {
            fprintf(stderr, "ERROR: Could not repair \"%s\" file.\n", FILENAME);
            return 0;
        }
    }
    //Creates a new configuration-file
    else if (status==-1)
    {
        status = create_config_file();
        if (status)
            printf("Original file not found. New \"%s\" file was created.\n", FILENAME);
        else
        {
            fprintf(stderr, "ERROR: Could not create \"%s\" file.\n", FILENAME);
            return 0;
        }
    }
    return 1;
}

/**
 * Creates a new configuration-file with the default values of the server.
 * */
int create_config_file(){
    FILE* f = fopen(FILENAME , "w");

    if(f){
        // Get current working directory
        char document_root[PATH_MAX];
        getcwd(document_root, sizeof(document_root));

        // Remove the last direcotry
        int len = strlen(document_root);
        int index = len - 1;
        while(document_root[index--] != '/'){}
        document_root[index + 1] = '\0';

        // Add www folder to the end of document root path
        strcat(document_root, "/www");

        fprintf(f, "SERVER_DOCUMENT_ROOT=%s\n", document_root);
        fprintf(f, "SERVER_NAME=Card_Server\n");
        fprintf(f, "SERVER_VERSION=0.2\n");
        fprintf(f, "SERVER_PORT=4444\n");
        fprintf(f, "SERVER_AS_DEAMON=0\n");
        fprintf(f, "SERVER_LOG=0\n");
        fprintf(f, "SERVER_SETTING=0\n");
        fprintf(f, "SERVER_JAIL=0\n");
    }
    else{
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}
