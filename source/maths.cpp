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

template<typename T>
Vec2<T> add_vec2(const Vec2<T> &a, const Vec2<T> &b)
{
	return {a.x + b.x, a.y + b.y};
}

template<typename T>
Vec2<T> subtract_vec2(const Vec2<T> &a, const Vec2<T> &b)
{
	return {a.x - b.x, a.y - b.y};
}

template<typename T>
Vec2<T> scale_vec2(const Vec2<T> &a, const T &value)
{
	return {a.x * value, a.y * value};
}

template<typename T>
Vec2<T> operator+(const Vec2<T> &a, const Vec2<T> &b)
{
	return add_vec2(a, b);
}

template<typename T>
Vec2<T> operator-(const Vec2<T> &a, const Vec2<T> &b)
{
	return subtract_vec2(a, b);
}

template<typename T>
Vec2<T> operator*(const Vec2<T> &a, const T &value)
{
	return scale_vec2(a, value);
}

template<typename T>
Vec2<T> Vec2<T>::operator+=(const Vec2<T> &other)
{
	return add_vec2(*this, other);
}

template<typename T>
Vec2<T> Vec2<T>::operator-=(const Vec2<T> &other)
{
	return subtract_vec2(*this, other);
}

template<typename T>
Vec2<T> Vec2<T>::operator*=(const T &value)
{
	return scale_vec2(*this, value);
}

