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
struct Client
{
	i32 port;
	i32 sock;
	u32 disconnected;
	//NOTE(stanisz): Uses KeyPressMsg for setting and getting
	// the data about each specific key.
	u32 keys_pressed_mask;
	i32 mouse_x;
	i32 mouse_y;
};

void recognize_client_key_press(Client* client, enum KeyIndex m);
void recognize_client_key_release(Client* client, enum KeyIndex m);

struct Tile
{
	IVec2 center_position;
	u32 color;
};

//TODO(stanisz): this should be serialized to handle endianness
struct World
{
	Tile tiles[100]; //NOTE(stanisz): 4x4 grid?
	u32 n_tiles;
	u32 tile_size; //NOTE(stanisz): this should be relative for each client - camera!
	u32 a; //NOTE(stanisz): test value

};

UVec2 get_tile_origin(u32 y_tile, u32 x_tile, u32 tile_size);
UVec2 get_tile_center(u32 x_tile, u32 y_tile, u32 tile_size);

u32 pack_color(u8 r, u8 g, u8 b, u8 a);

template<typename T>
T lerp(const T &v1, const T &v2, float t);

#endif
