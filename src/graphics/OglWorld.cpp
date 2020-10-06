
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

#include "Graphics.h"
#include "OglWorld.h"

OglWorld::OglWorld()
{
}

void OglWorld::create(int x, int y, int depth)
{
	resx = x;
	resy = y;
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	/* set video mode */
 	su = SDL_SetVideoMode( x,y, depth, SDL_OPENGLBLIT );
	if (su == NULL)
	{
		printf("Cannot initialize OpenGL\n");
		return;
	}

	/* OpenGL attributes */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

	/* set up lights */
	GLfloat am[]={1.2f,1.2f,1.2f,1.0f},lv[]={0.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,am);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,lv);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glClearColor(0.0,0.0,0.2,0.0);
}

void OglWorld::createFullScreen(int x, int y, int depth)
{
	resx = x;
	resy = y;
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	/* set video mode */
	su = SDL_SetVideoMode( x,y, depth, SDL_OPENGLBLIT | SDL_FULLSCREEN );
	if (su == NULL)
	{
		printf("Cannot initialize OpenGL\n");
		return;
	}
	glClearColor(0.0,0.0,0.2,0.0);

	/* OpenGL attributes */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

	/* set up lights */
	GLfloat am[]={1.2f,1.2f,1.2f,1.0f},lv[]={0.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,am);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,lv);
	glEnable(GL_LIGHTING);
}

void OglWorld::destroy()
{
}

void OglWorld::setLightPos(float x, float y, float z)
{

	GLfloat pos[4];
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	pos[3] = 0.0;
	glLightfv(GL_LIGHT0,GL_POSITION,pos);
	glEnable(GL_LIGHT0);
}

void OglWorld::beginRender()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


	glViewport( 0, 0, (GLint)resx, (GLint)resy );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)resx/(GLfloat)resy,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void OglWorld::endRender()
{
	SDL_GL_SwapBuffers();
}

bool OglWorld::checkMessages()
{
	SDL_Event e;

	while ( SDL_PollEvent(&e) )
	{
		if ( e.type == SDL_QUIT ) return true;

		if ( e.type == SDL_KEYDOWN )
		{
			if ( e.key.keysym.sym == SDLK_ESCAPE ) return true;
			if ( e.key.keysym.sym == 'q' ) return true;
		}
	}

	return false;
}
