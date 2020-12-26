#include "maths.hpp"

template<typename T>
Vec2<T> create_vec2(const T &x, const T &y)
{
	return {x, y};
}

template<typename T>
Vec2<T> create_vec2(const Vec2<T> &to_copy)
{
	return {to_copy.x, to_copy.y};
}

template<typename T>
Vec2<T> create_vec2(const T &x_and_y)
{
	return {x_and_y, x_and_y};
}
