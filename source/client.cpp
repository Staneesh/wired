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
#include "utils.cpp"
#include "maths.cpp"
#include "shared.cpp"

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

	if (event->type == SDL_KEYUP) {
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

void draw_colored_rectangle(u32* pixels, u32 window_width, u32 window_height, i32 x_center, i32 y_center, 
		u32 width, u32 height, u32 color)
{
	i32 x_start = x_center - width / 2;
	i32 y_start = y_center - height / 2; 
	if (x_start < 0) x_start = 0;
	if (y_start < 0) y_start = 0;

	
	u32 *pixel_start = pixels + y_start * window_width + x_start;
	for (u32 y = y_start; y < y_start + height; ++y)
	{
		if (y >= window_height) break;

		u32 *pixel = pixel_start;
		for (u32 x = x_start; x < x_start + width; ++x)
		{
			if (x >= window_width) break;

			*pixel++ = color;
		}
		pixel_start += window_width;
	}
}

void render_tiles(struct World *world_subset, u32 window_width, u32 window_height, u32* pixels)
{
	Tile *current_tile = world_subset->tiles;
	for (u32 tile_index = 0; tile_index < world_subset->n_tiles; ++tile_index, ++current_tile)
	{
		i32 camera_radius = 160;
		IVec2 relative_position = current_tile->center_position - IVec2(-camera_radius, camera_radius);
		LOG_INT(current_tile->center_position.x);
		LOG_INT(current_tile->center_position.y);

		draw_colored_rectangle(pixels, window_width, window_height,
				relative_position.x, relative_position.y,
				world_subset->tile_size, world_subset->tile_size, current_tile->color);
	}
}

void draw_visible_world_subset(struct World *world_subset, SDL_Texture *screen_texture, 
		u32 *pixels, SDL_Renderer *renderer, u32 window_width, u32 window_height)
{
	memset(pixels, 0xffffffff, window_width * window_height * sizeof(u32));

	render_tiles(world_subset, window_width, window_height, pixels);

	//NOTE(stanisz): shouldnt this be done using streaming texture, locking and unlocking? this is 
	// apparently slower, but locking results in a segfault.
	SDL_UpdateTexture(screen_texture, NULL, pixels, window_width * sizeof(u32));
	
	SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int main(int argc, char** argv)
{
	const u32 window_width = 1280, window_height = 720;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Wired", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			window_width, window_height, 0);

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
			SDL_TEXTUREACCESS_STATIC, window_width, window_height);
	if (!screen_texture)
		printf("%s\n", SDL_GetError());
		

	u32 *pixels = (u32*)malloc(sizeof(u32) * window_width * window_height);

	while (is_running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			is_running = handle_keyboard_for_client(&client, &event);
			handle_mouse_for_client(&client, &event);
		}

		draw_visible_world_subset(&world_subset, screen_texture, pixels, renderer, window_width, window_height);

		send(client.sock, &client, sizeof(client), 0);
		recv(client.sock, &world_subset, sizeof(world_subset), 0);
	}

	close(client.sock);

	SDL_Quit();

	free(pixels);

	return 0;
}
