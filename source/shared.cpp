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
	u32 x_center = tile_size / 2 + tile_size * x_tile;
	u32 y_center = tile_size / 2 + tile_size * y_tile;

	return create_vec2(x_center, y_center);
}

UVec2 get_tile_center(u32 x_tile, u32 y_tile, u32 tile_size)
{
	return get_tile_origin(x_tile, y_tile, tile_size) + create_vec2(tile_size / 2);



