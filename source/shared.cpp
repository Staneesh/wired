#include "shared.hpp"

void recognize_client_key_press(struct Client* client, enum KeyIndex m)
{
	client->keys_pressed_mask |= (1 << m); 	
}

void recognize_client_key_release(struct Client* client, enum KeyIndex m)
{
	client->keys_pressed_mask &= ~(1 << m); 
}

UVec2 get_tile_origin(u32 x_tile, u32 y_tile, u32 tile_size)
{
	u32 x_origin = tile_size * x_tile;
	u32 y_origin = tile_size * y_tile;

	return UVec2(x_origin, y_origin);
}

UVec2 get_tile_center(u32 x_tile, u32 y_tile, u32 tile_size)
{
	return get_tile_origin(x_tile, y_tile, tile_size) + UVec2(tile_size / 2);
}

