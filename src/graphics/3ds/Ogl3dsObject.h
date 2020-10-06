
#ifndef __OGL_VRML_OBJECT_H
#define __OGL_VRML_OBJECT_H

#include "Model_3DS.h"

class Ogl3dsObject : public OglObject
/* very short methods, used inline */
{
private:
	Model_3DS m;

public:
	Ogl3dsObject(const char *file_name)
	{
		if ( !m.Load(file_name) ) throw "Cannot load 3D model. File not found";
		m.lit = true;
		/*
		printf("%s, objs=%d, materials=%d, verts=%d, faces=%d\n",
			file_name, m.numObjects, m.numMaterials, m.totalVerts, m.totalFaces);
		*/
	}
	~Ogl3dsObject()
	{

	}

	void render()
	{
		m.Draw();
	}
};

#endif
