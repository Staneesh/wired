#include "shared.h"

void update_client_message(struct Client *client)
{
	bzero(client->message, sizeof(client->message));
	if (client->disconnected == 1)
	{
		client->message[DISCONNECTED] = '1';
	}
	if (client->key_up_pressed == 1)
	{
		client->message[KEYUP] = '1';
	}
}

