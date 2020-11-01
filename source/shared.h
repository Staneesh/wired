#ifndef SHARED_H
#define SHARED_H

enum MessageType
{
	DISCONNECTED,
	KEYUP,
};

typedef struct Client
{
	char message[256];
	i32 port;
	i32 sock;
	u32 disconnected;
	u32 key_up_pressed;
}Client;

void update_client_message(struct Client *client);

#endif
