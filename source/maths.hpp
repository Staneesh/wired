#ifndef MATHS_H
#define MATHS_H

template<typename T>
union Vec2
{
	struct 
	{
		T x, y;
	};

	T elements[2];
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


#endif
