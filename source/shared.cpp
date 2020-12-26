#include "shared.hpp"

void recognize_client_key_press(struct Client* client, enum KeyIndex m)
{
	client->keys_pressed_mask |= (1 << m); 	
}

void recognize_client_key_release(struct Client* client, enum KeyIndex m)
{
	client->keys_pressed_mask &= ~(1 << m); 
}

