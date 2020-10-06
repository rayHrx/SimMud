
/***************************************************************************************************
*
* SUBJECT:
*    A monitoring application for the Massive Multiplayer Online Game benchmark
*
* AUTHOR:
*    Mihai Paslariu
*    Politehnica University of Bucharest, Bucharest, Romania
*    mihplaesu@yahoo.com
*
* TIME AND PLACE:
*    University of Toronto, Toronto, Canada
*    March - August 2007
*
***************************************************************************************************/

#include "Utils.h"

void hsv2rgb(int h, int s, int v, int *r, int *g, int *b)
{
	float H = h;
	float S = s / 255.0;
	float V = v / 255.0;

	int Hi = (int)(H / 60) % 6;
	float f = H / 60.0 - Hi;
	float p = V * ( 1 - S );
	float q = V * ( 1 - f * S );
	float t = V * ( 1 - ( 1 - f ) * S );
	float R,G,B;

	switch ( Hi )
	{
		case 0: R = V, G = t, B = p; break;
		case 1: R = q, G = V, B = p; break;
		case 2: R = p, G = V, B = t; break;
		case 3: R = p, G = q, B = V; break;
		case 4: R = t, G = p, B = V; break;
		case 5: R = V, G = p, B = q; break;
		default: R = G = B = 1;
	}

	*r = (int)( R * 255 );
	*g = (int)( G * 255 );
	*b = (int)( B * 255 );
}
