
#ifndef SCENAVRML_H
#define SCENAVRML_H

#include "ObiectVRML.h"

class SCENA_VRML
{
private:
	int mtextura[MAXIM_TEXTURI];

	int nr_ob;
	OBIECT_VRML **ob;

	int nr_tex;
	char **tex;
public:
	SCENA_VRML();
	~SCENA_VRML();

	int Incarca(char *nume_fisier);
	void Afiseaza();
	void Gata();
	int NumarObiecte();
};

#endif
