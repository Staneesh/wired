#include <sys/socket.h>
#include <netinet/in.h>

#include <SDL2/SDL.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//NOTE(stanisz): Yes, this is a unity build. 
// I dont see any reasons to do incremental building, 
// so until then - its unity.
#include "utils.c"
#include "shared.c"

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

//TODO(stanisz): cleanup this code, its one million LOC 
u8 handle_keyboard_for_client(struct Client *client, SDL_Event *event)
{
	u8 result = 1;
	if (event->type == SDL_QUIT)
	{
		client->disconnected = 1;
		result = 0;
		return result;
	}

	if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_UP)
		{
			recognize_client_key_press(client, KEYUP);
		}
		if (event->key.keysym.sym == SDLK_DOWN)
		{
			recognize_client_key_press(client, KEYDOWN);
		}
		if (event->key.keysym.sym == SDLK_LEFT)
		{
			recognize_client_key_press(client, KEYLEFT);
		}
		if (event->key.keysym.sym == SDLK_RIGHT)
		{
			recognize_client_key_press(client, KEYRIGHT);
		}
	}

	if (event->type == SDL_KEYUP)
	{
		if (event->key.keysym.sym == SDLK_UP)
		{
			recognize_client_key_release(client, KEYUP);
		}
		if (event->key.keysym.sym == SDLK_DOWN)
		{
			recognize_client_key_release(client, KEYDOWN);
		}
		if (event->key.keysym.sym == SDLK_LEFT)
		{
			recognize_client_key_release(client, KEYLEFT);
		}
		if (event->key.keysym.sym == SDLK_RIGHT)
		{
			recognize_client_key_release(client, KEYRIGHT);
		}
	}

	return result;
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
	setup_socket(&client);

	World world_subset = {};

	u8 is_running = 1;

	while (is_running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			is_running = handle_keyboard_for_client(&client, &event);
		}

		send(client.sock, &client, sizeof(client), 0);
		recv(client.sock, &world_subset, sizeof(world_subset), 0);

		printf("Server says: \n");
		printf("%u\n", world_subset.a);


	}

	close(client.sock);

	SDL_Quit();

	return 0;
}
