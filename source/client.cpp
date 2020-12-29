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

void setup_socket(struct ClientInput* client_input)
{
	client_input->sock = socket(AF_INET, SOCK_STREAM, 0); 
	assert(client_input->sock != -1);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(client_input->port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	i32 connection_status = connect(client_input->sock, 
			(struct sockaddr *)&server_address, sizeof(server_address));

	assert(connection_status != -1);
}

void init_sdl(SDL_Window *window)
{
	UNUSED(window);
}

//TODO(stanisz): cleanup this code, its one million LOC 
u8 handle_keyboard_for_client(struct ClientInput *client_input, SDL_Event *event)
{
	u8 result = 1;
	if (event->type == SDL_QUIT)
	{
		result = 0;
		return result;
	}

	if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_UP)
		{
			recognize_client_key_press(client_input, KEYUP);
		}
		if (event->key.keysym.sym == SDLK_DOWN)
		{
			recognize_client_key_press(client_input, KEYDOWN);
		}
		if (event->key.keysym.sym == SDLK_LEFT)
		{
			recognize_client_key_press(client_input, KEYLEFT);
		}
		if (event->key.keysym.sym == SDLK_RIGHT)
		{
			recognize_client_key_press(client_input, KEYRIGHT);
		}
	}

	if (event->type == SDL_KEYUP) {
		if (event->key.keysym.sym == SDLK_UP)
		{
			recognize_client_key_release(client_input, KEYUP);
		}
		if (event->key.keysym.sym == SDLK_DOWN)
		{
			recognize_client_key_release(client_input, KEYDOWN);
		}
		if (event->key.keysym.sym == SDLK_LEFT)
		{
			recognize_client_key_release(client_input, KEYLEFT);
		}
		if (event->key.keysym.sym == SDLK_RIGHT)
		{
			recognize_client_key_release(client_input, KEYRIGHT);
		}
	}

	return result;
}

void handle_mouse_for_client(struct ClientInput *client_input, SDL_Event *event)
{
	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		recognize_client_key_press(client_input, MOUSEPRESSED);
	}
	if (event->type == SDL_MOUSEBUTTONUP)
	{
		recognize_client_key_release(client_input, MOUSEPRESSED);
	}
	if (event->type == SDL_MOUSEMOTION)
	{
		client_input->mouse_x = event->motion.x;
		client_input->mouse_y = event->motion.y;
	}
}

void draw_colored_rectangle(u32* pixels, i32 window_width, i32 window_height, i32 x_center, i32 y_center, 
		i32 width, i32 height, u32 color)
{
	i32 x_start = x_center - width / 2;
	i32 y_start = y_center - height / 2; 
	if (x_start + width < 0 || x_start >= window_width) return; 
	if (y_start + height < 0 || y_start >= window_height) return; 

	if (x_start < 0)
	{
		width -= -x_start;
		x_start = 0;
	}
	if (y_start < 0)
	{
		height -= -y_start;
		y_start = 0;
	}

	
	u32 *pixel_start = pixels + y_start * window_width + x_start;
	for (i32 y = y_start; y < y_start + height; ++y)
	{
		if (y >= window_height) break;

		u32 *pixel = pixel_start;
		for (i32 x = x_start; x < x_start + width; ++x)
		{
			if (x >= window_width) break;

			*pixel++ = color;
		}
		pixel_start += window_width;
	}
}

void update_camera_position(const ClientInput &input, FVec2 *camera_position, float delta_time)
{
	float camera_speed = 1;
	FVec2 camera_movement = FVec2(0.0f);

	if (input.keys_pressed_mask & (1<<KEYUP))
	{
		camera_movement += FVec2(0, 1);
	}
	if (input.keys_pressed_mask & (1<<KEYDOWN))
	{
		camera_movement += FVec2(0, -1);
	}
	if (input.keys_pressed_mask & (1<<KEYLEFT))
	{
		camera_movement += FVec2(-1, 0);
	}
	if (input.keys_pressed_mask & (1<<KEYRIGHT))
	{
		camera_movement += FVec2(1, 0);
	}

	float movement_len = length_vec2(camera_movement);

	if (movement_len > 0.5f)
	{
		camera_movement *= 1.0f / length_vec2(camera_movement);	
	}	

	*camera_position += camera_movement * camera_speed * delta_time;
}

void render_tiles(struct World *world_subset, const FVec2 &camera_position, 
		u32 window_width, u32 window_height, u32* pixels)
{
	Tile *current_tile = world_subset->tiles;
	for (u32 tile_index = 0; tile_index < world_subset->n_tiles; ++tile_index, ++current_tile)
	{
		IVec2 truncated_camera_pos = IVec2(camera_position.x, camera_position.y);
		IVec2 relative_position = current_tile->center_position - IVec2(-720/2, 720/2) - truncated_camera_pos;
		relative_position.y *= -1;

		draw_colored_rectangle(pixels, window_width, window_height,
				relative_position.x, relative_position.y,
				world_subset->tile_size, world_subset->tile_size, current_tile->color);
	}
}

void draw_visible_world_subset(struct World *world_subset, SDL_Texture *screen_texture, 
		u32 *pixels, SDL_Renderer *renderer, u32 window_width, u32 window_height,
		const FVec2 &camera_position)
{
	memset(pixels, 0xffffffff, window_width * window_height * sizeof(u32));

	render_tiles(world_subset, camera_position, window_width, window_height, pixels);

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

	struct ClientInput client_input = {};
	
	client_input.port = 9002;
	if (argc > 1)
	{
		client_input.port = atoi(argv[1]);
	}
	setup_socket(&client_input);

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

	FVec2 camera_position = FVec2(0.0f);

	u64 last_time = SDL_GetPerformanceCounter();
	float delta_time = 0;

	while (is_running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			is_running = handle_keyboard_for_client(&client_input, &event);
			handle_mouse_for_client(&client_input, &event);
		}

		update_camera_position(client_input, &camera_position, delta_time);

		draw_visible_world_subset(&world_subset, screen_texture, pixels, renderer, 
				window_width, window_height, camera_position);

		send(client_input.sock, &client_input, sizeof(client_input), 0);
		recv(client_input.sock, &world_subset, sizeof(world_subset), 0);

		u64 current_time = SDL_GetPerformanceCounter();
		delta_time = (current_time - last_time) * 1000.0f / SDL_GetPerformanceFrequency();

		i32 useconds_60fps = 16666;
		i32 useconds_dt = (i32)(1000.0f * delta_time);

		if (useconds_dt < useconds_60fps)
		{
			LOG("Starting sync...");
			while (useconds_dt < useconds_60fps)
			{
				i32 useconds_vsync = useconds_60fps - useconds_dt; 
				LOG_INT(useconds_vsync);
				usleep(useconds_vsync);
				
				current_time = SDL_GetPerformanceCounter();
				delta_time = (current_time - last_time) * 1000.0f / SDL_GetPerformanceFrequency();
				useconds_dt = (i32)(1000.0f * delta_time);
			}
			LOG_FLOAT(delta_time);
			LOG("Sync ended.");
		}
		else
		{
			//NOTE(stanisz): missed frame
			LOG("Missed frames!");
		}

		last_time = current_time;
	}

	close(client_input.sock);

	SDL_Quit();

	free(pixels);

	return 0;
}
