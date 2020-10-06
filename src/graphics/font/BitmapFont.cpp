/*This source code copyrighted by Lazy Foo' Productions (2004-2007) and may not be redestributed without written permission.*/

/* Code from "http://lazyfoo.net/SDL_tutorials/index.php" (modified) */

#ifdef WIN32
#include <windows.h>
#endif

#include <SDL.h>
#include "BitmapFont.h"

const int RED_BACK = 0x0;
const int BLUE_BACK = 0x0;
const int GREEN_BACK = 0x0;

SDL_Surface *load_image(const char* filename )
{
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;
	loadedImage = SDL_LoadBMP( filename );

	if( loadedImage != NULL )
	{
		optimizedImage = SDL_DisplayFormat( loadedImage );
		SDL_FreeSurface( loadedImage );

		if( optimizedImage != NULL )
			SDL_SetColorKey( optimizedImage, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, RED_BACK, GREEN_BACK, BLUE_BACK ) );
	} else throw "Cannot load font (file not found)";
	return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface( source, clip, destination, &offset );
}

Uint32 get_pixel32( int x, int y, SDL_Surface *surface )
{
	Uint32 *pixels = (Uint32 *)surface->pixels;
	return pixels[ ( y * surface->w ) + x ];
}

BitmapFont::BitmapFont( const char* filename )
{
	build_font(load_image( filename ));
}

void BitmapFont::build_font( SDL_Surface *surface )
{
	if( surface == NULL ) return;

	bitmap = surface;
	Uint32 bgColor = SDL_MapRGB( bitmap->format, RED_BACK, GREEN_BACK, BLUE_BACK );

	int cellW = bitmap->w / 16;
	int cellH = bitmap->h / 16;
	int currentChar = 0;

	for( int rows = 0; rows < 16; rows++ )
	{
		for( int cols = 0; cols < 16; cols++ )
		{
			chars[ currentChar ].x = cellW * cols;
			chars[ currentChar ].y = cellH * rows;

			chars[ currentChar ].w = cellW;
			chars[ currentChar ].h = cellH;

			for( int pCol = 0; pCol < cellW; pCol++ )
			{
				for( int pRow = 0; pRow < cellH; pRow++ )
				{
					int pX = ( cellW * cols ) + pCol;
					int pY = ( cellH * rows ) + pRow;

					if( get_pixel32( pX, pY, bitmap ) != bgColor )
					{
						chars[ currentChar ].x = pX;

						pCol = cellW;
						pRow = cellH;
					}
				}
			}

			for( int pCol_w = cellW - 1; pCol_w >= 0; pCol_w-- )
			{
				for( int pRow_w = 0; pRow_w < cellH; pRow_w++ )
				{
					int pX = ( cellW * cols ) + pCol_w;
					int pY = ( cellH * rows ) + pRow_w;

					if( get_pixel32( pX, pY, bitmap ) != bgColor )
					{
						chars[ currentChar ].w = ( pX - chars[ currentChar ].x ) + 1;

						pCol_w = -1;
						pRow_w = cellH;
					}
				}
			}

			currentChar++;
		}
	}
}

void BitmapFont::render( int x, int y, char* text, SDL_Surface *surface )
{
	int X = x, Y = y;

	if( bitmap != NULL )
	{
		for( int show = 0; text[ show ] != '\0'; show++ )
		{
			if( text[ show ] == ' ' )
			{
				X += bitmap->w / 32;
			} else if( text[ show ] == '\n' ) {
				Y += bitmap->h / 16;
				X = x;
			} else {
				int ascii = (unsigned char)text[ show ];
				apply_surface( X, Y, bitmap, surface, &chars[ ascii ] );
				X += chars[ ascii ].w + 1;
			}
		}
	}
}
