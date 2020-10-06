
/***************************************************************************************************
*
* SUBJECT:
*    A Benckmark for Massive Multiplayer Online Games
*    Game Server and Client
*
* AUTHOR:
*    Mihai Paslariu
*    Politehnica University of Bucharest, Romania
*    mihplaesu@yahoo.com
*
* TIME AND PLACE:
*    University of Toronto, Toronto, Canada
*    March - August 2007
*
***************************************************************************************************/

#ifndef __GENERAL_H
#define __GENERAL_H

/*
CONVENTIONS:
- critical errors are thrown and caught in main, minor errors are return by functions
or displayed as warnings
*/

/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

/* Windows headers */
#ifdef WIN32
#include <windows.h>
#include <set>
#endif

/* SDL headers */
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_net.h>

/* My Headers */
#include "Settings.h"
#include "Constants.h"
#include "Structs.h"

/* Namespaces */
/***********************************************************************/

using namespace std;

/* Abstract classes */
/***********************************************************************/

class Module
{
public:
	int f_id;
	virtual ~Module() {};	/* to prevent a warning from g++ */
	virtual void run() {};	/* all modules must have a run function */
};

int module_thread(void *data);

/* Macros */
/***********************************************************************/

#ifndef min
#define min(x,y) ((x<y)?(x):(y))
#endif

#ifndef max
#define max(x,y) ((x>y)?(x):(y))
#endif

#endif
