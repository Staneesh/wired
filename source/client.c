//NOTE(stanisz): Yes, this is a unity build. 
// I dont see any reasons to do incremental building, 
// so until then - its unity.
#include "utils.c"

#include <sys/socket.h>
#include <netinet/in.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	i32 port = 9002;
	if (argc > 1)
	{
		port = atoi(argv[1]);
	}

	i32 network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0); 
	assert(network_socket != -1);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	i32 connection_status = connect(network_socket, 
			(struct sockaddr *)&server_address, sizeof(server_address));

	assert(connection_status != -1);

	char client_message[256] = "HELLO!\0";
	char server_message[256] = {};	
	while (1)
	{
		send(network_socket, &client_message, sizeof(client_message), 0);
		recv(network_socket, &server_message, sizeof(server_message), 0);

		server_message[sizeof(server_message)-1] = '\0';
		printf("Client on port [%d]: Server says: %s\n", port, server_message);
	}

	close(network_socket);

	return 0;
}
