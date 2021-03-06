#include "maths.hpp"

template<typename T>
Vec2<T> create_vec2(const T &x, const T &y)
{
	return {x, y};
}

template<typename T>
Vec2<T> create_vec2(const Vec2<T>& to_copy)
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
void Vec2<T>::operator=(const Vec2<T> &other)
{
	x = other.x;
	y = other.y;
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
void Vec2<T>::operator+=(const Vec2<T> &other)
{
	*this = add_vec2(*this, other);
}

template<typename T>
void Vec2<T>::operator-=(const Vec2<T> &other)
{
	*this = subtract_vec2(*this, other);
}

template<typename T>
void Vec2<T>::operator*=(const T &value)
{
	*this = scale_vec2(*this, value);
}

template<typename T>
Vec2<T>::Vec2(const T &v1, const T &v2)
{
	x = v1;
	y = v2;
}

template<typename T>
Vec2<T>::Vec2(const Vec2<T> &to_copy)
{
	x = to_copy.x;
	y = to_copy.y;
}

template<typename T>
Vec2<T>::Vec2(const T &x_and_y)
{
	x = x_and_y;
	y = x_and_y;
}

template<typename T>
float length_vec2(const Vec2<T> &a)
{
	return sqrt((float)a.x * a.x + (float)a.y * a.y);
}
