#include <stdio.h>
#include <stdlib.h>

#include "../include/thread.h"
#include "../include/socket.h"

#define NUM_THREADS 5

void* printMessage(void* threadid)
{
    printf("\n%d: Hello World!\n",*(int*) threadid);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	//init_threads(3);
	set_ip_type(0);
	set_port(4444);
	set_protocol(0);
	createSocket(10);

	Client client = connectToClient();

	char buf[8] = "Hello!\n";
	if(send(client.socket, buf, 8, 0) == -1) {
		fprintf(stderr, "ERROR: Can not send 'Hello' to the client.\n");
	}

	close(client.socket);
	closeServer();
	/*for (int rc = 0, i = 0; i < NUM_THREADS; i++)
	{
		rc = new_thread(printMessage);
		if (rc) {
			fprintf(stderr, "ERROR: pthread_create() returned %d\n", rc);
			exit(-1);
		}
	
	}*/

	//terminate_threads();

    return 0;
}
