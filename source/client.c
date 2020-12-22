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

void handle_mouse_for_client(struct Client *client, SDL_Event *event)
{
	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		recognize_client_key_press(client, MOUSEPRESSED);
	}
	if (event->type == SDL_MOUSEBUTTONUP)
	{
		recognize_client_key_release(client, MOUSEPRESSED);
	}
	if (event->type == SDL_MOUSEMOTION)
	{
		client->mouse_x = event->motion.x;
		client->mouse_y = event->motion.y;
	}
}

void draw_colored_rectangle(u32* pixels, u32 x_center, u32 y_center, 
		u32 width, u32 height, u32 color)
{
	u32 x_start = x_center - width / 2;
	u32 y_start = y_center - height / 2; 
	u32 *pixel_start = pixels + y_start * 1280 + x_start;
	for (u32 y = y_start; y < y_start + height; ++y)
	{
		u32 *pixel = pixel_start;
		for (u32 x = x_start; x < x_start + width; ++x)
		{
			*pixel++ = color;
		}
		pixel_start += 1280;
	}
}

void draw_visible_world_subset(SDL_Texture *screen_texture, u32 *pixels, SDL_Renderer *renderer)
{
	//write to pixels
	u32 *pixel = pixels;
	for (u32 y = 0; y < 720; ++y)
	{
		for (u32 x = 0; x < 1280; ++x)
		{
			*pixel++ = 0xff0000ff;
		}
	}

	draw_colored_rectangle(pixels, 20, 20, 10, 10, 0x00ff00ff);
	//memset(pixels, 255, 720 * 1280 * sizeof(u32));

	//NOTE(stanisz): shouldnt this be done using streaming texture, locking and unlocking? this is 
	// apparently slower, but locking results in a segfault.
	SDL_UpdateTexture(screen_texture, NULL, pixels, 1280 * sizeof(u32));

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Wired", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			1280, 720, 0);

	UNUSED(argc);
	UNUSED(argv);

	struct Client client = {};
	
	client.disconnected = 1;
	client.port = 9002;
	if (argc > 1)
	{
		client.port = atoi(argv[1]);
	}
	setup_socket(&client);

	//NOTE(stanisz): This contains only the data visible to the client.
	// Some elements are not discovered by the client probably, and should
	// remain hidden. The server computes what elements are visible.
	World world_subset = {};

	u8 is_running = 1;

	//RESEARCH(stanisz): What flags should be passed to SDL_CreateRenderer and SDL_CreateTexture?
	// Do they matter performance-wise if the rendering is cpu based?
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer)
		printf("%s\n", SDL_GetError());

	SDL_Texture *screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888,
			SDL_TEXTUREACCESS_STATIC, 1280, 720);
	if (!screen_texture)
		printf("%s\n", SDL_GetError());
		

	u32 *pixels = (u32*)malloc(sizeof(u32) * 1280 * 720);

	while (is_running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			is_running = handle_keyboard_for_client(&client, &event);
			handle_mouse_for_client(&client, &event);
		}

		draw_visible_world_subset(screen_texture, pixels, renderer);

		send(client.sock, &client, sizeof(client), 0);
		recv(client.sock, &world_subset, sizeof(world_subset), 0);
	}

	close(client.sock);

	SDL_Quit();

	free(pixels);

	return 0;
}
