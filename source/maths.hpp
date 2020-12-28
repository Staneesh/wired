#ifndef MATHS_H
#define MATHS_H

#include <cmath>

template<typename T>
union Vec2
{
	struct 
	{
		T x, y;
	};

	T elements[2];

	void operator=(const Vec2<T> &other);
	Vec2<T>(){}

	Vec2<T> operator+=(const Vec2<T> &other);
	Vec2<T> operator-=(const Vec2<T> &other);
	Vec2<T> operator*=(const T &value);

	Vec2<T>(const T &v1, const T &v2);
	Vec2<T>(const Vec2<T> &to_copy);
	Vec2<T>(const T &x_and_y);
};

using FVec2 = Vec2<float>;
using DVec2 = Vec2<double>;
using IVec2 = Vec2<int>;
using UVec2 = Vec2<unsigned>;

template<typename T>
Vec2<T> create_vec2(const T &x, const T &y);

template<typename T>
Vec2<T> create_vec2(const Vec2<T> &to_copy);

template<typename T>
Vec2<T> create_vec2(const T &x_and_y);

template<typename T>
Vec2<T> add_vec2(const Vec2<T> &a, const Vec2<T> &b);

//ROBUSTNESS(stanisz): What happens if T is unsigned? I want 
// the compiler (or the program) to warn the user that there
// can be unsigned subtraction there!
template<typename T>
Vec2<T> subtract_vec2(const Vec2<T> &a, const Vec2<T> &b);

template<typename T>
Vec2<T> scale_vec2(const Vec2<T> &a, const T &value);

template<typename T>
Vec2<T> operator+(const Vec2<T> &a, const Vec2<T> &b);

template<typename T>
Vec2<T> operator-(const Vec2<T> &a, const Vec2<T> &b);

template<typename T>
Vec2<T> operator*(const Vec2<T> &a, const T &value);

template<typename T>
float length_vec2(const Vec2<T> &a);
#endif
