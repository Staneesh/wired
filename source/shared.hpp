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
	u32 color;
	
} Tile;

//TODO(stanisz): this should be serialized to handle endianness
typedef struct World
{
	struct Tile tiles[16]; //NOTE(stanisz): 4x4 grid?
	u32 n_tiles;
	u32 tile_size;
	u32 a; //NOTE(stanisz): test value

} World;

#endif
