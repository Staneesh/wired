#ifndef SHARED_H
#define SHARED_H

enum KeyIndex
{
	KEYUP,
	KEYDOWN,
	KEYLEFT,
	KEYRIGHT,
	MOUSEPRESSED,
};

//TODO(stanisz): this should be serialized to handle endianness
typedef struct Client
{
	i32 port;
	i32 sock;
	u32 disconnected;
	//NOTE(stanisz): Uses KeyPressMsg for setting and getting
	// the data about each specific key.
	u32 keys_pressed_mask;
	i32 mouse_x;
	i32 mouse_y;
} Client;

void recognize_client_key_press(struct Client* client, enum KeyIndex m);
void recognize_client_key_release(struct Client* client, enum KeyIndex m);

typedef struct Tile
{
	//TODO(stanisz): TODO!
	u32 x, y;
	u32 color;
	
} Tile;

//TODO(stanisz): this should be serialized to handle endianness
typedef struct World
{
	u32 a;

	struct Tile *tiles;
	u32 n_tiles;

} World;

#endif
