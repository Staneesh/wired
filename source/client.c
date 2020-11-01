//NOTE(stanisz): Yes, this is a unity build. 
// I dont see any reasons to do incremental building, 
// so until then - its unity.
#include "utils.c"
#include "shared.c"

#include <sys/socket.h>
#include <netinet/in.h>

#include <SDL2/SDL.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void setup_socket(struct Client* client)
{
	client->sock = socket(AF_INET, SOCK_STREAM, 0); 
	assert(client->sock != -1);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(client->port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	i32 connection_status = connect(client->sock, 
			(struct sockaddr *)&server_address, sizeof(server_address));

	assert(connection_status != -1);

	client->disconnected = 0;
}

void init_sdl(SDL_Window *window)
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Wired", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			1280, 720, 0);
	UNUSED(window);
}

int main(int argc, char** argv)
{
	SDL_Window *window = 0;
	init_sdl(window);

	UNUSED(argc);
	UNUSED(argv);

	struct Client client = {};
	//TODO(stanisz): is it okay to assume disconnected by default
	// until checked for connection (accept)?
	client.disconnected = 1;
	client.port = 9002;
	if (argc > 1)
	{
		client.port = atoi(argv[1]);
	}
	bzero(client.message, sizeof(client.message));
	setup_socket(&client);

	char server_message[256] = {};	

	u8 is_running = 1;

	while (is_running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				is_running = 0;
				client.disconnected = 1;
			}
		}

		update_client_message(&client);

		send(client.sock, client.message, sizeof(client.message), 0);
		recv(client.sock, &server_message, sizeof(server_message), 0);

		server_message[sizeof(server_message)-1] = '\0';
		printf("Client on port [%d]: Server says: %s\n", client.port, server_message);


	}

	close(client.sock);

	SDL_Quit();

	return 0;
}
