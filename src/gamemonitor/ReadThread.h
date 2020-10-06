
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

#ifndef __READ_THREAD_H
#define __READ_THREAD_H

/***************************************************************************************************
*
* Thread that performs reading into global variables
*
***************************************************************************************************/

class ReadThread : public wxThread
{
private:
	int **layout2;
	int **players_per_region2;
	SlowReader *sr;

public:
	ReadThread(SlowReader *sr);
	void *Entry();
	int **new_int_matrix(int m, int n);
};

#endif
