
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

#ifndef __SLOW_READER_H
#define __SLOW_READER_H

/***************************************************************************************************
*
* SlowReader class
* - reads from a file but sleeps when there is nothing to read
*
***************************************************************************************************/

class SlowReader
{
private:
	FILE *f;
public:
	bool open(char *file_name);
	void close();
	void read(void *buffer, int count);
	int size();
	void seek(int pos);
};

#endif
