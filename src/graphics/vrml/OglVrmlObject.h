
#ifndef __OGL_VRML_OBJECT_H
#define __OGL_VRML_OBJECT_H

#include "../Graphics.h"
#include "ScenaVRML.h"

class OglVrmlObject : public OglObject
/* very short methods, used inline */
{
private:
	SCENA_VRML s;

public:
	OglVrmlObject(char *file_name)
	{
		if ( s.Incarca(file_name) == 0 ) throw "Cannot load 3D model";
	}
	~OglVrmlObject()
	{
	}

	void render()
	{
		s.Afiseaza();
	}
};

#endif
