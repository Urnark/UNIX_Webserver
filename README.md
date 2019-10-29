Webserver(R) Version 2.12 13/10/2019 - created by Rebecca Schümann, Christoffer Åleskog

Usage notes
-------------------------------------------------------------------------------------------------
- This program is a webserver for ubuntu, which supports
html/txt/css/image files to build a website of your choice.

- tested on Ubuntu 12.0/16.0/18.4


supports
-------------------------------------------------------------------------------------------------
- HTTP Versions: 0.9/1.0/1.1
- html 4.0


Implementations
-------------------------------------------------------------------------------------------------
- requestmethods: thread / fork
- can be run as daemonprocess
- security: realpath / jail
(be aware that jail can only be run as root, so be sure you have the rights to start it as such.)


Installation notes
-------------------------------------------------------------------------------------------------
- use the make command in the webserver folder to create
server with whom you can execute your webserver.


Options for running the server
-------------------------------------------------------------------------------------------------

server [-dlrj] [-p port_number] [-s thread | -s fork]	

	-h                      dislay help.
	-p [0-65535]            overwrites the default port used by the server on initialization.
	-d                      starts the server as deamon.
	-l                      creates a logfile to write in.
	-s [thread | fork ]     sets the requesthandling method of the server.
	-r                      repaires the configuration file and restores the default values.
	-j                      run server with chroot, jail.


These Options can also be adjusted over the configurationfile .lab3-config

SERVER_DOCUMENT_ROOT=[Enter the path to your wwwfolder/folder with your webfiles here] 
SERVER_NAME=[Name of the Server]
SERVER_VERSION=[2.12]
SERVER_PORT=[Portnumber]
SERVER_AS_DEAMON=[0 for no daemon/ 1 for run as a deamon]
SERVER_LOG=[0 for write to syslog/ 1 for create a logfile to write to]
SERVER_SETTING=[0 for execute with threads/ 1 for execute with fork()]
SERVER_JAIL=[0 for realpath/ 1 for run in a jail]
