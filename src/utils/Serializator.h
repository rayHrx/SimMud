
/***************************************************************************************************
*
* SUBJECT:
*    A Benckmark for Massive Multiplayer Online Games
*    Game Server and Client
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

#ifndef __SERIALIZATION_H
#define __SERIALIZATION_H

#define BUFFER_ADD	4096

/*
	TODO: some mechanism to return errors
*/

class Serializator
{
protected:
	int capacity;
	int size;
	char *buffer;

	int position;
public:
	bool can_grow;

public:
	Serializator();
	Serializator(char *buffer, int size);
	Serializator(char *buffer, int size, int cap);
	virtual ~Serializator();

	virtual int getSize();
	virtual char* getBuffer();

	void clear();
	void rewind();
	void jump(int bytes);

	Serializator& operator<<( char x );
	Serializator& operator>>( char &x );

	Serializator& operator<<( int x );
	Serializator& operator>>( int &x );

	Serializator& operator<<( char *x );
	Serializator& operator>>( char *&x );	/* assumes x has enough space */

	void putBytes(char *data, int len);
	void getBytes(char *data, int len);
	char *getBytesPtr(int len);
};

#endif
