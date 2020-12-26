#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//NOTE(stanisz): Yes, this is a unity build. 
// I dont see any reasons to do incremental building, 
// so until then - its unity.
#include "utils.c"
#include "shared.c"

//NOTE(stanisz): Listeners and senders are separated now,
// maybe it would be more efficient to create a thread 
// and have it communicate both ways with the client.
struct ListenerWork
{
	u32 port;
	i32 sock;
	Client client_data;
};

struct SenderWork
{
	u32 port;
	i32 sock;
	World world_subset;
};

void listener(struct ListenerWork* work)
{
	assert(
			recv(work->sock, &work->client_data,
				sizeof(work->client_data), 0) != -1
			);
}

void listen_to_clients(struct Client *clients, const u32 n_listeners)
{
	struct ListenerWork works[n_listeners];

	pthread_t thread_ids[n_listeners];

	for(u32 i = 0; i < n_listeners; ++i)
	{
		works[i].port = 9002 + i;
		works[i].sock = clients[i].sock;

		pthread_create(&thread_ids[i], 0, (void *)&listener, &works[i]);
	}

	for (u32 i = 0; i < n_listeners; ++i)
	{
		pthread_join(thread_ids[i], 0);
	}

	for(u32 i = 0; i < n_listeners; ++i)
	{
		//TODO(stanisz): copying a struct doesnt work apparently
		//clients[i] = works[i].client_data;
		//FIXME(stanisz): why?

		
		clients[i].disconnected = works[i].client_data.disconnected;
		clients[i].keys_pressed_mask = works[i].client_data.keys_pressed_mask;
		clients[i].mouse_x = works[i].client_data.mouse_x;
		clients[i].mouse_y = works[i].client_data.mouse_y;
		
	}
}

void sender(struct SenderWork* work)
{
	assert(
			send(work->sock, &work->world_subset, 
				sizeof(work->world_subset), 0) != -1
			);
}

void send_to_clients(struct Client *clients, struct World worlds[8], 
		const u32 n_clients)
{
	struct SenderWork works[n_clients];

	pthread_t thread_ids[n_clients];

	for(u32 i = 0; i < n_clients; ++i)
	{
		works[i].port = 9002 + i;
		works[i].sock = clients[i].sock;
		works[i].world_subset = worlds[i];

		pthread_create(&thread_ids[i], 0, (void *)&sender, &works[i]);
	}

	for (u32 i = 0; i < n_clients; ++i)
	{
		pthread_join(thread_ids[i], 0);
	}
}

void setup_sockets(struct Client *clients, u32 n_clients)
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
				(clients[i].sock = accept(sock, 
					(struct sockaddr *)&client_address, 
					&len)) != -1
				);
	}
}

void cleanup_sockets(struct Client *clients, u32 n_clients)
{
	//RESEARCH(stanisz): Is it okay to close sockets, that
	// are 0 (not created at all)?
	for (u32 i = 0; i < n_clients; ++i)
	{
		close(clients[i].sock);
	}
}

void print_clients(struct Client clients[8], u32 n_clients) {
	for (u32 i = 0; i < n_clients; ++i)
	{
		if (clients[i].disconnected != 0)
		{
			LOG("Client disconnected.");
		}
		if (clients[i].keys_pressed_mask != 0)
		{
			LOG_UINT(clients[i].keys_pressed_mask);
		}
	}
}

//NOTE(stanisz): This should probably update already-existing world subsetsdynamically, although i am not sure that everything can be implemented faster that way.
void compute_world_subset(struct World worlds[8], u32 n_worlds)
{
	for (u32 i = 0; i < n_worlds; ++i)
	{
		worlds[i].a = i + 112;
		worlds[i].n_tiles = 16;
		worlds[i].tile_size = 720 / 4;   
		for (u32 j = 0; j < worlds[i].n_tiles; ++j)
		{
			u32 r = 255 / (j+1);
			u32 color = 0x000000ff | (r << 28);
			worlds[i].tiles[j].color = color;
		}
	}
}

int main(int argc, char** argv)
{
	u32 n_clients = 1;

	if (argc == 2)
	{
		n_clients = atoi(argv[1]);
	}

	struct Client clients[8];
	setup_sockets(clients, n_clients);

	struct World worlds[8] = {};

	while(1)
	{
		listen_to_clients(clients, n_clients);
		//print_clients(clients, n_clients);
		compute_world_subset(worlds, n_clients);
		send_to_clients(clients, worlds, n_clients);
	}
	
	cleanup_sockets(clients, n_clients);

	return 0;
}
