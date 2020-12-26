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

typedef Vec2<float> FVec2;
typedef Vec2<unsigned> UVec2;
typedef Vec2<int> IVec2;
typedef Vec2<double> DVec2;

template<typename T>
Vec2<T> create_vec2(const T &x, const T &y);

template<typename T>
Vec2<T> create_vec2(const Vec2<T> &to_copy);

template<typename T>
Vec2<T> create_vec2(const T &x_and_y);

#endif
