//////////////////////////////////////////////////////////////////////
//
// http://www.garagegames.com/
//
//////////////////////////////////////////////////////////////////////
//
// OpenGL Texture Class
// by: Matthew Fairfax
//
// GLTexture.cpp: implementation of the GLTexture class.
// This class loads a texture file and prepares it
// to be used in OpenGL. It can open a bitmap or a
// targa file. The min filter is set to mipmap b/c
// they look better and the performance cost on
// modern video cards in negligible. I leave all of
// the texture management to the application. I have
// included the ability to load the texture from a
// Visual Studio resource. The bitmap's id must be
// be surrounded by quotation marks (i.e. "Texture.bmp").
// The targa files must be in a resource type of "TGA"
// (including the quotes). The targa's id must be
// surrounded by quotation marks (i.e. "Texture.tga").
//
// Usage:
// GLTexture tex;
// GLTexture tex1;
// GLTexture tex3;
//
// tex.Load("texture.bmp"); // Loads a bitmap
// tex.Use();				// Binds the bitmap for use
//
// tex1.LoadFromResource("texture.tga"); // Loads a targa
// tex1.Use();				 // Binds the targa for use
//
// // You can also build a texture with a single color and use it
// tex3.BuildColorTexture(255, 0, 0);	// Builds a solid red texture
// tex3.Use();				 // Binds the targa for use
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include "GLTexture.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../texture/texture.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLTexture::GLTexture()
{

}

GLTexture::~GLTexture()
{

}

void GLTexture::Load(char *name)
{
	texturename = strdup(name);
	LoadBMP(texturename);
}

void GLTexture::Use()
{
	glEnable(GL_TEXTURE_2D);								// Enable texture mapping
	glBindTexture(GL_TEXTURE_2D, texture[0]);				// Bind the texture as the current one
}

void GLTexture::Finish()
{
	glDisable(GL_TEXTURE_2D);
}

void GLTexture::LoadBMP(char *name)
{
	texture[0] = LoadBitmap(name, &width, &height);
}

void GLTexture::BuildColorTexture(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char data[12];	// a 2x2 texture at 24 bits

	// Store the data
	for(int i = 0; i < 12; i += 3)
	{
		data[i] = r;
		data[i+1] = g;
		data[i+2] = b;
	}

	// Generate the OpenGL texture id
	glGenTextures(1, &texture[0]);

	// Bind this texture to its id
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Use mipmapping filter
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Generate the texture
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 2, 2, GL_RGB, GL_UNSIGNED_BYTE, data);
}
