#ifndef SHARED_H
#define SHARED_H

enum MessageType
{
	DISCONNECTED,
};

typedef struct Client
{
	char message[256];
	i32 port;
	i32 sock;
	u32 disconnected;
}Client;

void update_client_message(struct Client *client);

#endif
