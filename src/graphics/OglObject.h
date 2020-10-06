
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

#ifndef __OGL_OBJECT_H
#define __OGL_OBJECT_H

class OglObject
{
public:
	virtual ~OglObject() {};
	virtual void render() = 0;

	virtual void render(float x, float y)
	{
		glPushMatrix();
		glTranslatef(x, 0.0, y);
		render();
		glPopMatrix();
	}

	virtual void render(float x, float y, float scale)
	{
		glPushMatrix();
		glTranslatef(x, 0.0, y);
		glScalef(scale,scale,scale);
		render();
		glPopMatrix();
	}

	virtual void render(float x, float y, float scale, float rotate)
	{
		glPushMatrix();
		glTranslatef(x, 0.0, y);
		glScalef(scale,scale,scale);
		glRotatef(rotate,0,1,0);
		render();
		glPopMatrix();
	}

	virtual void render(float x, float y, float scale,
		float rotate1, float rotate2)
	{
		glPushMatrix();
		glTranslatef(x, 0.0, y);
		glScalef(scale,scale,scale);
		glRotatef(rotate1,0,1,0);
		glRotatef(rotate2,1,0,0);
		render();
		glPopMatrix();
	}
};

#endif
