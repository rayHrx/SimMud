
/***************************************************************************************************
*
* SUBJECT:
*    A Benckmark for Massive Multiplayer Online Games
*    Game Server and Client
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

#ifndef __OGL_WORLD_H
#define __OGL_WORLD_H

class OglWorld
{
private:
	SDL_Surface *su;
	int resx,resy;

public:
	OglWorld();

	void create(int x, int y, int depth);
	void createFullScreen(int x, int y, int depth);
	void destroy();

	inline SDL_Surface *getScreen() { return su; }

	void setLightPos(float x, float y, float z);
	void beginRender();
	void endRender();
	bool checkMessages();	/* returns true if the application must finish */
};

#endif
