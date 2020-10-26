//NOTE(stanisz): Yes, this is a unity build. 
// I dont see any reasons to do incremental building, 
// so until then - its unity.
#include "utils.c"

#include <sys/socket.h>
#include <netinet/in.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	i32 network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0); 
	assert(network_socket != -1);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	i32 connection_status = connect(network_socket, 
			(struct sockaddr *)&server_address, sizeof(server_address));

	assert(connection_status != -1);

	char server_response[256];
	recv(network_socket, &server_response, sizeof(server_response), 0);

	printf("Server: %s\n", server_response);

	close(network_socket);

	return 0;
}
