#include "shared.h"

void update_client_message(struct Client *client)
{
	bzero(client->message, sizeof(client->message));
	if (client->disconnected == 1)
	{
		client->message[DISCONNECTED] = '1';
	}
	client->message[KEYS_PRESSED_MASK] = client->keys_pressed_mask;
}

void recognize_client_key_press(struct Client* client, enum KeyIndex m)
{
	client->keys_pressed_mask |= (1 << m); 	
}

void recognize_client_key_release(struct Client* client, enum KeyIndex m)
{
	client->keys_pressed_mask &= ~(1 << m); 
}
