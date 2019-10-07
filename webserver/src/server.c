#include "../include/server.h"

void *client_thread(void *args)
{
    int thread_id = ((Thread_args *)args)->id;
    Client *client = &((Thread_args *)args)->client;
    int is_fork = ((Thread_args *)args)->is_fork;

    printf("\n%d: Start Connection!\n", ((Thread_args *)args)->id);

    Request_t request = request_received(client);
    gather_response_information(&request, client);
    free_headers(&request.headers);

    //close(client->socket);
    shutdown(client->socket, SHUT_WR);
    if (is_fork == 0)
    {
        thread_manager_exit_thread(args);
    }
}

void handle_threading()
{
    thread_manager_init_threads(START_NUM_THREADS);
    int running = 1;
    while (shoudStopRunning(running))
    {
        int accept_connection;
        Client client = connectToClient(&accept_connection);
        if (accept_connection == 1)
        {
            Thread_args *args = malloc(sizeof(Thread_args));
            args->client = client;
            args->is_fork == 0;
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
}

void handle_fork()
{
    pid_t origin = getpid();
    pid_t p;
    p = fork();
    printf("pid: %d", p);
    printf("pid: %d", origin);
    int running = 1;
    while (shoudStopRunning(running))
    {
        if (p != origin)
        {
            int accept_connection;
            Client client = connectToClient(&accept_connection);
            if (accept_connection == 1)
            {
                Thread_args *args = malloc(sizeof(Thread_args));
                args->client = client;
                pid_t p_two;
                p_two = fork();
                printf("pid: %d", p_two);
                if (p_two != 0)
                {
                    args->is_fork == 1;
                    client_thread((void *)args);
                    exit(0);
                }
            }
        }
        else
        {
            //handle childsignals
        }
    }
    request_stop_reciving_data = 1;
}

void create_a_deamon()
{
    //code here 
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

    request_init();

    
        if (setting == 1)
        {
            if (deamon == 1)
                {
                    create_a_deamon();
                }
            handle_fork();
        }
        else if(setting == 0)
        {
            if (deamon == 1)
                {
                    create_a_deamon();
                }
            handle_threading();
        }
    }

    closeServer();
}
