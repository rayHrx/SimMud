
/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* Windows headers */
#ifdef WIN32
#include <windows.h>
#endif

/* OpenGL headers */
#include <GL/gl.h>
#include <GL/glu.h>

#include "../texture/texture.h"
#include "ScenaVRML.h"

void CreateTexture(int mtextura[], char* tex, int i)
{
	int x = 0, y = 0;
	mtextura[i] = LoadBitmap(tex, &x,&y);
	printf("%d %s (%dx%d)\n", mtextura[i], tex, x,y);
}

SCENA_VRML::SCENA_VRML()
{
	nr_ob=0;
	ob=NULL;
	nr_tex=0;
	tex=NULL;
}

SCENA_VRML::~SCENA_VRML()
{
	Gata();
}

int SCENA_VRML::Incarca(char *nume_fisier)
{
	int l,i,j,p;
	FILE *f;
	char *tot,*b[MAXIM_OBIECTE_IN_SCENA];

	f=fopen(nume_fisier,"rb");
	if (f==NULL) { printf("Cannot find %s\n", nume_fisier); return 0;}
	fseek(f,0,2);
	l=ftell(f);
	rewind(f);
	tot=new char[l+1];
	fread(tot,1,l,f);
	tot[l]=0;
	fclose(f);

	// cauta cate obiecte sunt
	p=0;
	nr_ob=0;
	b[0]=tot;
	for (i=0;i<l;i++)
	{
		if (tot[i]=='{') p++;
		if (tot[i]=='}')
		{
			p--;
			if (p==0)
			{
				tot[i]=0;
				nr_ob++;
				if (nr_ob==MAXIM_OBIECTE_IN_SCENA) break;
				b[nr_ob]=tot+i+1;
			}
		}
	}

	// pregateste vectorul cu texturile
	tex=new char*[MAXIM_TEXTURI];
	nr_tex=0;

	// incarca obiectele
	ob=new OBIECT_VRML*[nr_ob];
	for (i=0;i<nr_ob;i++)
	{
		ob[i]=new OBIECT_VRML(mtextura);
		ob[i]->Incarca(b[i]);
		// adauga textura in vector daca e nevoie

		if ( ob[i]->textura != NULL )
		{
			for (j=0;j<nr_tex;j++)
				if (!strcmp(tex[j],ob[i]->textura)) break;
			if (j==nr_tex) // nu exista textura
				if (nr_tex<MAXIM_TEXTURI)
				{
					tex[nr_tex]=new char[strlen(ob[i]->textura)+1];
					strcpy(tex[nr_tex],ob[i]->textura);
					nr_tex++;
				}
			ob[i]->IndiceTextura(j);
		}
	}

	char prefix[1024],texture_name[1024];
	strncpy(prefix, nume_fisier, 1024);
	p = -1;
	for ( j = 0; prefix[j] != 0; j++ )
		if ( prefix[j] == '/' || prefix[j] == '\\' ) p = j;
	if ( p > 0 ) prefix[p+1] = 0;

	// incarca texturile
	for (j=0;j<nr_tex;j++)
	{
		sprintf(texture_name, "%s%s", prefix, tex[j]);
		CreateTexture(mtextura,texture_name,j);
	}

	delete[] tot;
	return nr_ob;
}

void SCENA_VRML::Afiseaza()
{
	if (ob==NULL) return;

	for (int i=0;i<nr_ob;i++)
		ob[i]->Afiseaza();
}

void SCENA_VRML::Gata()
{
	if (ob==NULL) {nr_ob=0;return;}
	for (int i=0;i<nr_ob;i++)
	{
		if (ob[i]!=NULL)
			delete ob[i];
	}
	delete[] ob;
	nr_ob=0;
}

int SCENA_VRML::NumarObiecte()
{
	return nr_ob;
}
