#include "shared.hpp"

void recognize_client_key_press(struct ClientInput* client, enum KeyIndex m)
{
	client->keys_pressed_mask |= (1 << m); 	
}

void recognize_client_key_release(struct ClientInput* client, enum KeyIndex m)
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

u32 pack_color(u8 r, u8 g, u8 b, u8 a)
{
	u32 result = 0;

	result |= ((u32)r << 24);
	result |= ((u32)g << 16);
	result |= ((u32)b << 8);
	result |= ((u32)a << 0);


	return result;
}

template<typename T>
T lerp(const T &v1, const T &v2, float t)
{
	return (1.0f - t) * v1 + t * v2;
}
