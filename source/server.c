//NOTE(stanisz): Yes, this is a unity build. 
// I dont see any reasons to do incremental building, 
// so until then - its unity.
#include "utils.c"

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct ListenerWork
{
	u32 port;
	u8 client_message[256];
};

struct SenderWork
{
	u32 port;
	u8 server_message[256];
};

void listener(struct ListenerWork* work)
{
	memset(work->client_message, 0, 256);

	i32 sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(work->port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));

	listen(sock, 5);

	int client_socket = accept(sock, NULL, NULL);

	recv(client_socket, work->client_message, sizeof(work->client_message), 0);

	close(sock);
	
}

void listen_to_clients(const u32 n_listeners)
{
	struct ListenerWork works[n_listeners];

	pthread_t thread_ids[n_listeners];

	for(u32 i = 0; i < n_listeners; ++i)
	{
		works[i].port = 9002 + i;

		pthread_create(&thread_ids[i], 0, (void *)&listener, &works[i]);
	}

	for (u32 i = 0; i < n_listeners; ++i)
	{
		pthread_join(thread_ids[i], 0);
	}

	for(u32 i = 0; i < n_listeners; ++i)
	{
		printf("From port [%d]: %s\n", 9002 + i, works[i].client_message);
	}
}

void sender(struct SenderWork* work)
{
	i32 sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in client_address;
	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(work->port);
	client_address.sin_addr.s_addr = INADDR_ANY;

	bind(sock, (struct sockaddr *)&client_address, sizeof(client_address));

	listen(sock, 5);

	int client_socket = accept(sock, NULL, NULL);

	send(client_socket, work->server_message, sizeof(work->server_message), 0);

	close(sock);
}

void send_to_clients(const u32 n_senders)
{
	struct SenderWork works[n_senders];

	pthread_t thread_ids[n_senders];

	for(u32 i = 0; i < n_senders; ++i)
	{
		works[i].port = 9002 + i;
		u8 msg[50] = "SERVER TALKS!\0";
		memcpy(&works[i].server_message, msg, sizeof(msg));
		pthread_create(&thread_ids[i], 0, (void *)&sender, &works[i]);
	}

	for (u32 i = 0; i < n_senders; ++i)
	{
		pthread_join(thread_ids[i], 0);
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Usage: ./server N_CLIENTS\n");
		return -1;
	}

	const u32 n_clients = atoi(argv[1]);
	
	listen_to_clients(n_clients);
	send_to_clients(n_clients);

	return 0;
}
