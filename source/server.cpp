#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <SDL2/SDL.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//NOTE(stanisz): Yes, this is a unity build. 
// I dont see any reasons to do incremental building, 
// so until then - its unity.
#include "utils.cpp"
#include "maths.cpp"
#include "shared.cpp"

//NOTE(stanisz): Listeners and senders are separated now,
// maybe it would be more efficient to create a thread 
// and have it communicate both ways with the client.
struct ListenerWork
{
	u32 port;
	i32 sock;
	ClientInput client_data;
};

struct SenderWork
{
	u32 port;
	i32 sock;
	World world_subset;
};

void* listener(void *work_pass)
{
	ListenerWork *work = (ListenerWork *)work_pass;
	assert(
			recv(work->sock, &work->client_data,
				sizeof(work->client_data), 0) != -1
			);
	return nullptr;
}

void listen_to_clients(struct ClientInput *client_inputs, const u32 n_listeners)
{
	struct ListenerWork works[n_listeners];

	pthread_t thread_ids[n_listeners];

	for(u32 i = 0; i < n_listeners; ++i)
	{
		works[i].port = 9002 + i;
		works[i].sock = client_inputs[i].sock;

		pthread_create(&thread_ids[i], 0, &listener, &works[i]);
	}

	for (u32 i = 0; i < n_listeners; ++i)
	{
		pthread_join(thread_ids[i], 0);
	}

	for(u32 i = 0; i < n_listeners; ++i)
	{
		//TODO(stanisz): copying a struct normally doesnt work apparently.
		// I have to store old values of sock and port, because setting them
		// causes the following send to fail.
		u32 old_port = client_inputs[i].port;
		u32 old_sock = client_inputs[i].sock;
		client_inputs[i] = works[i].client_data;
		client_inputs[i].sock = old_sock;
		client_inputs[i].port = old_port;
		//FIXME(stanisz): why?
	}
}

void* sender(void *work_pass)
{
	SenderWork *work = (SenderWork *)work_pass;
	assert(
			send(work->sock, &work->world_subset, 
				sizeof(work->world_subset), 0) != -1
			);

	return nullptr;
}

void send_to_clients(struct ClientInput *client_inputs, struct World worlds[8], 
		const u32 n_clients)
{
	struct SenderWork works[n_clients];

	pthread_t thread_ids[n_clients];

	for(u32 i = 0; i < n_clients; ++i)
	{
		works[i].port = 9002 + i;
		works[i].sock = client_inputs[i].sock;
		works[i].world_subset = worlds[i];

		pthread_create(&thread_ids[i], 0, &sender, &works[i]);
	}

	for (u32 i = 0; i < n_clients; ++i)
	{
		pthread_join(thread_ids[i], 0);
	}
}

void setup_sockets(struct ClientInput *client_inputs, u32 n_clients)
{
	for (u32 i = 0; i < n_clients; ++i)
	{
		i32 sock = socket(AF_INET, SOCK_STREAM, 0);
		assert(sock != -1);

		int opt = 1;
		//RESEARCH(stanisz): What are those flags exactly?
		assert(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
					&opt, sizeof(opt)) != -1);

		struct sockaddr_in client_address = {};
		client_address.sin_family = AF_INET;
		client_address.sin_port = htons(9002 + i);
		client_address.sin_addr.s_addr = INADDR_ANY;

		assert(bind(sock, 
					(struct sockaddr *)&client_address, 
					sizeof(client_address))
				!= -1);

		//RESEARCH(stanisz): Whats the 5?
		assert(listen(sock, 5) != -1);

		socklen_t len = sizeof(client_address);

		//NOTE(stanisz): Accept happens here, so before everything starts
		// the clients need to all be accepted. Right...?
		assert(
				(client_inputs[i].sock = accept(sock, 
					(struct sockaddr *)&client_address, 
					&len)) != -1
				);
	}
}

void cleanup_sockets(struct ClientInput *clients, u32 n_clients)
{
	//RESEARCH(stanisz): Is it okay to close sockets, that
	// are 0 (not created at all)?
	for (u32 i = 0; i < n_clients; ++i)
	{
		close(clients[i].sock);
	}
}
void print_clients(struct ClientInput clients[8], u32 n_clients) {
	for (u32 i = 0; i < n_clients; ++i)
	{
		if (clients[i].keys_pressed_mask != 0)
		{
			LOG_UINT(clients[i].keys_pressed_mask);
		}
	}
}

void mouse_tile_selection(Tile *current_tile, i32 tile_size, ClientInput *client_input)
{
	IVec2 upper_left = current_tile->center_position - IVec2(tile_size/2);
	IVec2 lower_right = upper_left + IVec2(tile_size);
	i32 x = client_input->mouse_x;
	i32 y = client_input->mouse_y;

	if (x > upper_left.x && x < lower_right.x)
	{
		if (y > upper_left.y && y < lower_right.y)
		{
			current_tile->color = pack_color(0, 0, 0, 255);
		}
	}
}

//NOTE(stanisz): This should probably update already-existing world subsetsdynamically, although i am not sure that everything can be implemented faster that way.
void compute_world_subsets(World *true_world, ClientInput client_inputs[8], World world_subsets[8], u32 n_worlds)
{
	u32 CLIENT_INITIAL_VISIBILITY = 300;
	for (u32 world_subset_index = 0; world_subset_index < n_worlds; ++world_subset_index)
	{
		world_subsets[world_subset_index].n_tiles = 0;

		for (u32 true_world_tile_index = 0; true_world_tile_index < true_world->n_tiles; ++true_world_tile_index)
		{
			Tile current_tile = true_world->tiles[true_world_tile_index];

			if (length_vec2(current_tile.center_position) < CLIENT_INITIAL_VISIBILITY)
			{
				mouse_tile_selection(&current_tile, true_world->tile_size, &client_inputs[world_subset_index]);
				world_subsets[world_subset_index].tiles[world_subsets[world_subset_index].n_tiles++] = current_tile;
			}
		}

		world_subsets[world_subset_index].tile_size = true_world->tile_size;   
	}
}

World generate_world()
{
	World result = {};
	i32 tiles_on_side = 10;
	result.n_tiles = tiles_on_side * tiles_on_side;	
	result.tile_size = 50;

	for (u32 i_tile = 0; i_tile < result.n_tiles; ++i_tile)
	{
		i32 y_level = -(i32)i_tile / tiles_on_side + tiles_on_side / 2; 
		i32 x_level = i_tile % tiles_on_side - tiles_on_side / 2;

		IVec2 origin_coordinates = IVec2(x_level, y_level) * (int)result.tile_size;

		float color_factor = (float)i_tile / result.n_tiles;
		u32 r = lerp(70, 255, color_factor);
		result.tiles[i_tile].color = pack_color(r, 0, 255 - r, 255);
		result.tiles[i_tile].center_position = origin_coordinates + IVec2(result.tile_size / 2, -(i32)result.tile_size/2);
	}

	return result;
}

int main(int argc, char** argv)
{
	u32 n_clients = 1;

	if (argc == 2)
	{
		n_clients = atoi(argv[1]);
	}

	ClientInput client_inputs[8];
	setup_sockets(client_inputs, n_clients);

	World true_world = generate_world();

	World world_subsets[8] = {};

	while(1)
	{
		listen_to_clients(client_inputs, n_clients);
		//print_clients(clients, n_clients);
		compute_world_subsets(&true_world, client_inputs, world_subsets, n_clients);
		send_to_clients(client_inputs, world_subsets, n_clients);
	}
	
	cleanup_sockets(client_inputs, n_clients);

	return 0;
}
