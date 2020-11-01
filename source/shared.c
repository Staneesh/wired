#include "shared.h"

void set_message(char *client_message, enum MessageType t)
{
	client_message[t] = 1;
}

void update_client_message(struct Client *client)
{
	if (client->disconnected)
	{
		set_message(client->message, DISCONNECTED);
	}
}

