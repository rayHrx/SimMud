
#ifndef UTIL_H
#define UTIL_H

struct PUNCT2D
{
	float x,y;
};

struct PUNCT3D
{
	float x,y,z;
};

struct TRIUNGHI
{
	int a,b,c;
};

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif 

#define radian(x) (x*M_PI)/180
#define grad(x) (x*180)/M_PI

#endif
