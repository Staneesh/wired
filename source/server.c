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
	//NOTE(stanisz): This message will be recieved by the server.
	u8 client_message[256];
};

struct SenderWork
{
	u32 port;
	i32 sock;
	//NOTE(stanisz): This message will be recieved by the client.
	u8 server_message[256];
};

void listener(struct ListenerWork* work)
{
	memset(work->client_message, 0, 256);

	assert(
			recv(work->sock, work->client_message,
				sizeof(work->client_message), 0) != -1
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
		printf("From port [%d]: %s\n", 9002 + i, works[i].client_message);
		memcpy(clients[i].message, works[i].client_message,
				sizeof(works[i].client_message));
	}
}

void sender(struct SenderWork* work)
{
	assert(
			send(work->sock, work->server_message, 
				sizeof(work->server_message), 0) != -1
			);
}

void send_to_clients(struct Client *clients, const u32 n_clients)
{
	struct SenderWork works[n_clients];

	pthread_t thread_ids[n_clients];

	for(u32 i = 0; i < n_clients; ++i)
	{
		works[i].port = 9002 + i;
		works[i].sock = clients[i].sock;

		u8 msg[50] = "SERVER TALKS!\0";
		memcpy(&works[i].server_message, msg, sizeof(msg));

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
		//TODO(stanisz): What are those flags exactly?
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

		//TODO(stanisz): Whats the 5?
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
	//TODO(stanisz): Is it okay to close sockets, that
	// are 0 (not created at all)?
	for (u32 i = 0; i < n_clients; ++i)
	{
		close(clients[i].sock);
	}
}

void parse_clients(struct Client *clients, u32 n_clients)
{
	for (u32 i = 0; i < n_clients; ++i)
	{
		if (clients[i].message[DISCONNECTED] == '1')
		{
			clients[i].disconnected = 1;
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

	//NOTE(stanisz): Each client (out of 8 maximum) has their 
	// 256 chars of data.
	while(1)
	{
		listen_to_clients(clients, n_clients);

		parse_clients(clients, n_clients);

		for (u32 i = 0; i < n_clients; ++i)
		{
			if (clients[i].disconnected == 1)
			{
				LOG("Client disconnected.");
			}
		}

		send_to_clients(clients, n_clients);
	}
	
	cleanup_sockets(clients, n_clients);

	return 0;
}
