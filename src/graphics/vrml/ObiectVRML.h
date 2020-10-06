
#ifndef OBIECTVRML_H
#define OBIECTVRML_H

#include "Util.h"

#define MAXIM_OBIECTE_IN_SCENA 256
#define MAXIM_TEXTURI MAXIM_OBIECTE_IN_SCENA

class OBIECT_VRML
{
private:
	char *nume;
	float cr,cg,cb;// culoare

	float tx,ty,tz;		// stranslatie
	float ax,ay,az,ur;	// rotatie
	float sx,sy,sz;		// scalare

	int *mtextura;

public:
	char *textura;
	int ind_text;

private:
	PUNCT3D *pc;int nr_pc;
	TRIUNGHI *tr;PUNCT3D *norm;int nr_tr;
	PUNCT2D *tc;int nr_tc;
	TRIUNGHI *tci;int nr_tci;

public:
	OBIECT_VRML(int *mtextura);
	~OBIECT_VRML();
	bool Incarca(char *tot);
	void Afiseaza();
	void Gata();
	void IndiceTextura(int i);
};

#endif
