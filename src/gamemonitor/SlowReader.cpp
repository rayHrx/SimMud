/***************************************************************************************************
*
* SUBJECT:
*    A monitoring application for the Massive Multiplayer Online Game benchmark
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

#include <stdio.h>
#ifndef WIN32
	#include <unistd.h>
#else
	#include <windows.h>
#endif

#include "SlowReader.h"

bool SlowReader::open(char *file_name)
{
	f = fopen(file_name, "rb");
	if ( f == NULL ) return false;
	return true;
}

void SlowReader::close()
{
	fclose(f);
}

void SlowReader::read(void *buffer, int count)
{
	int pos = 0;
	int remaining = count;
	int ret = 0;
	while ( remaining > 0 )
	{
		ret = fread((char*)buffer + pos, 1, remaining, f);
		pos += ret;
		remaining -= ret;
		if ( remaining > 0 )
			#ifndef WIN32
			usleep(10000);
			#else
			Sleep(10);
			#endif

	}
}

int SlowReader::size()
{
	fseek(f, 0, SEEK_END);
	int l = (int)ftell(f);
	fseek(f, 0, SEEK_SET);
	return l;
}

void SlowReader::seek(int pos)
{
	fseek(f, pos, SEEK_SET);
}
