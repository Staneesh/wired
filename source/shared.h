#ifndef SHARED_H
#define SHARED_H

enum KeyIndex
{
	KEYUP,
	KEYDOWN,
	KEYLEFT,
	KEYRIGHT,
};

enum MessageType
{
	DISCONNECTED,
	KEYS_PRESSED_MASK,
};

typedef struct Client
{
	/*NOTE(stanisz): Data layout of 'message':
		BYTES			CONTENT 
		0 - 3			Control, 'disconnected' field
		4 - 7			Keyboard, 'keys_pressed_mask' field
	*/
	u32 message[256];
	i32 port;
	i32 sock;
	u32 disconnected;
	//NOTE(stanisz): Uses KeyPressMsg for setting and getting
	// the data about each specific key.
	u32 keys_pressed_mask;
}Client;

void update_client_message(struct Client *client);

void recognize_client_key_press(struct Client* client, enum KeyIndex m);
void recognize_client_key_release(struct Client* client, enum KeyIndex m);


#endif
