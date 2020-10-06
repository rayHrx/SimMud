
#ifndef __BITMAP_FONT_H
#define __BITMAP_FONT_H

class BitmapFont
{
private:
	SDL_Surface *bitmap;
	SDL_Rect chars[256];

public:
	BitmapFont( const char *filename );

	void build_font( SDL_Surface *surface );
	void render( int x, int y, char* text, SDL_Surface *surface );
};

#endif
