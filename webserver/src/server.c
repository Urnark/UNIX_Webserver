#include "../include/server.h"

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

	if(deamon == 0)
	{
		if(setting == 0)
		{
			exec_threading();
		}
		else
		{
			exec_preforking();
		}
	}
	else
	{
		//finish the deamon code here
		pid_t p;
    	p = fork();
		if(p==0)
		{
			//child here
			if(setting == 0)
			{
				exec_threading();
			}
			else
			{
				exec_preforking();
			}
		}
		else
		{
			exit(0); //kill the parent? 
		}
	}
	closeServer();
}