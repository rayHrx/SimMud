
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

#include "ObiectVRML.h"

OBIECT_VRML::OBIECT_VRML(int *mtextura)
{
	pc=NULL;tc=NULL;tr=NULL;tci=NULL;norm=NULL;
	nr_pc=nr_tr=nr_tc=nr_tci=0;
	tx=ty=tz=0;
	ax=ay=0;az=1;ur=0;
	sx=sy=sz=1;
	nume=NULL;
	textura=NULL;
	cr=cg=cb=0;
	this->mtextura = mtextura;
}

OBIECT_VRML::~OBIECT_VRML()
{
	Gata();
}

bool OBIECT_VRML::Incarca(char *tot)
{
	char *p,*q,*r;
	int i,k;

	// numele
	p=strstr(tot,"DEF");
	if (p==NULL) goto gata_rau;
	p+=3;while (*p==' ') p++;
	q=p;while (*q!=' ') q++;
	*q=0;
	nume=new char[q-p+1];
	strcpy(nume,p);p=q+1;

	// translatie
	q=strstr(p,"translation");
	if (q!=NULL)
	{
		q+=12;
		sscanf(q,"%f%f%f",&tx,&ty,&tz);
	}

	// rotatie
	q=strstr(p,"rotation");
	if (q!=NULL)
	{
		q+=9;
		sscanf(q,"%f%f%f%f",&ax,&ay,&az,&ur);
	}

	// scalare
	q=strstr(p,"scale");
	if (q!=NULL)
	{
		q+=6;
		sscanf(q,"%f%f%f",&sx,&sy,&sz);
	}

	// culoare
	q=strstr(p,"diffuseColor");
	if (q!=NULL)
	{
		q=q+13;
		sscanf(q,"%f%f%f",&cr,&cg,&cb);
	} else cr=cg=cb=0;

	//textura
	q=strstr(p,"ImageTexture");
	if (q!=NULL)
	{
		p=q;
		while (*q!=0)
		{
			if (*q=='"') break;
			q++;
		}
		if (*q!=0)
		{
			q++;
			r=q;
			while (*q!=0)
			{
				if (*r=='"') break;
				r++;
			}
			*r=0;
			textura=new char[r-q+1];
			strcpy(textura,q);
		}
		p=r+1;
	}

	// coordonatele varfurilor
	p=strstr(p,"coord");
	if (p==NULL) goto gata_rau;
	p=strchr(p,'[');if (p==NULL) goto gata_rau;
	p++;
	q=strchr(p,']');if (q==NULL) goto gata_rau;
	nr_pc=1;
	for (r=p;r<q;r++)
		if (*r==',') nr_pc++;
	pc=new PUNCT3D[nr_pc];

	for (i=0;i<nr_pc;i++)
	{
		sscanf(p,"%f %f %f",&pc[i].x,&pc[i].y,&pc[i].z);
		if (i!=nr_pc-1)
			p=strchr(p,',')+1;
		else
			p=strchr(p,']')+1;
	}

	// coordonatele texturii
	q=strstr(p,"texCoord");
	if (q!=NULL)
	{
		p=q;
		p=strchr(p,'[');if (p==NULL) goto gata_rau;
		p++;
		q=strchr(p,']');if (q==NULL) goto gata_rau;
		nr_tc=1;
		for (r=p;r<q;r++)
			if (*r==',') nr_tc++;
		tc=new PUNCT2D[nr_tc];

		for (i=0;i<nr_tc;i++)
		{
			sscanf(p,"%f %f",&tc[i].x,&tc[i].y);
			if (i!=nr_tc-1)
				p=strchr(p,',')+1;
			else
				p=strchr(p,']')+1;
		}
	} else textura=NULL;

	// triunghiuri
	p=strstr(p,"coordIndex");if (p==NULL) goto gata_rau;
	p=strchr(p,'[');if (p==NULL) goto gata_rau;
	p++;
	q=strchr(p,']');if (q==NULL) goto gata_rau;
	nr_tr=1;
	for (r=p;r<q;r++)
		if (*r==',') nr_tr++;
	nr_tr/=4;
	tr=new TRIUNGHI[nr_tr];

	for (i=0;i<nr_tr;i++)
	{
		sscanf(p,"%d, %d, %d, %d",&tr[i].a,&tr[i].b,&tr[i].c,&k);
		if (i!=nr_tr-1)
			p=strchr(p,',')+1,p=strchr(p,',')+1,p=strchr(p,',')+1,p=strchr(p,',')+1;
		else
			p=strchr(p,']')+1;
	}

	// triunghiuri coordonate textura
	q=strstr(p,"texCoordIndex");
	if (q!=NULL)
	{
		p=q;
		p=strchr(p,'[');if (p==NULL) goto gata_rau;
		p++;
		q=strchr(p,']');if (q==NULL) goto gata_rau;
		nr_tci=1;
		for (r=p;r<q;r++)
			if (*r==',') nr_tci++;
		nr_tci/=4;
		tci=new TRIUNGHI[nr_tci];

		for (i=0;i<nr_tci;i++)
		{
			sscanf(p,"%d, %d, %d, %d",&tci[i].a,&tci[i].b,&tci[i].c,&k);
			if (i!=nr_tci-1)
				p=strchr(p,',')+1,p=strchr(p,',')+1,p=strchr(p,',')+1,p=strchr(p,',')+1;
			else
				p=strchr(p,']')+1;
		}
	} else textura=NULL;

	// calculeaza normale
	norm = new PUNCT3D[nr_tr];
	for (i=0; i<nr_tr; i++)
	{
		double x0,x1,x2,y0,y1,y2,z0,z1,z2;
		x0=pc[tr[i].a].x;
		x1=pc[tr[i].b].x;
		x2=pc[tr[i].c].x;
		y0=pc[tr[i].a].y;
		y1=pc[tr[i].b].y;
		y2=pc[tr[i].c].y;
		z0=pc[tr[i].a].z;
		z1=pc[tr[i].b].z;
		z2=pc[tr[i].c].z;
		norm[i].x = (y1-y0)*(z2-z1)-(y2-y1)*(z1-z0);
		norm[i].y = (x2-x1)*(z1-z0)-(x1-x0)*(z2-z1);
		norm[i].z = (x1-x0)*(y2-y1)-(x2-x1)*(y1-y0);
	}

	return true;

gata_rau:
	return false;
}

void OBIECT_VRML::Afiseaza()
{
	GLfloat culoare[]={cr,cg,cb,1.0};
	if (textura!=NULL)
	{
		glBindTexture(GL_TEXTURE_2D,mtextura[ind_text]);
	} else {
		glMaterialf(GL_FRONT, GL_SHININESS, 50);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,culoare);
	}

	glPushMatrix();
	glTranslatef(tx,ty,tz);
	glRotatef( (float)grad(ur), ax, ay, az);
	glScalef(sx,sy,sz);
	glBegin(GL_TRIANGLES);

	for (int i=0;i<nr_tr;i++)
	{
		if (norm!=NULL) glNormal3f(norm[i].x, norm[i].y, norm[i].z);

		if (textura!=NULL)
			glTexCoord2f(tc[tci[i].a].x,tc[tci[i].a].y);
		glColor3ub((int)(cr*255.0),(int)(cg*255.0),(int)(cb*255.0));
		glVertex3f(pc[tr[i].a].x,pc[tr[i].a].y,pc[tr[i].a].z);

		if (textura!=NULL)
			glTexCoord2f(tc[tci[i].b].x,tc[tci[i].b].y);
		glColor3ub((int)(cr*255.0),(int)(cg*255.0),(int)(cb*255.0));
		glVertex3f(pc[tr[i].b].x,pc[tr[i].b].y,pc[tr[i].b].z);

		if (textura!=NULL)
			glTexCoord2f(tc[tci[i].c].x,tc[tci[i].c].y);
		glColor3ub((int)(cr*255.0),(int)(cg*255.0),(int)(cb*255.0));
		glVertex3f(pc[tr[i].c].x,pc[tr[i].c].y,pc[tr[i].c].z);
	}

	glEnd();
	glPopMatrix();
}

void OBIECT_VRML::Gata()
{
	if (pc!=NULL) delete pc;
	pc=NULL;
	if (tc!=NULL) delete tc;
	tc=NULL;
	if (tr!=NULL) delete tr;
	tr=NULL;
	if (norm!=NULL) delete norm;
	norm = NULL;
	if (tci!=NULL) delete tci;
	tci=NULL;
	nr_pc=nr_tr=nr_tc=nr_tci=0;
	if (nume!=NULL) delete nume;
	nume=NULL;
	if (textura!=NULL) delete textura;
	textura=NULL;
	cr=cg=cb=0;
	tx=ty=tz=0;
}

void OBIECT_VRML::IndiceTextura(int i)
{
	ind_text=i;
}
