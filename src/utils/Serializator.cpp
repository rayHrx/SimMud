
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

#include <stdlib.h>	/* malloc/free used instead of new/delete for convenience */
#include <string.h>
#include <stdio.h>
#include "Serializator.h"

Serializator::Serializator()
{
	capacity = 0;
	size = 0;
	buffer = NULL;
	position = 0;
	can_grow = true;
}

Serializator::Serializator(char *buffer, int size)
{
	this->buffer = buffer;
	this->size = size;
	position = 0;
	capacity = size;
	can_grow = false;
}

Serializator::Serializator(char *buffer, int size, int cap)
{
	this->buffer = buffer;
	this->size = size;
	position = 0;
	capacity = cap;
	can_grow = false;
}

Serializator::~Serializator()
{
	/* free buffer only if was not passed as argument to the constructor */
	if ( buffer != NULL && can_grow ) free(buffer);
	buffer = NULL;
}

int Serializator::getSize()
{
	return size;
}

char* Serializator::getBuffer()
{
	return buffer;
}

void Serializator::clear()
{
	size = 0;
	position = 0;
}

void Serializator::rewind()
{
	position = 0;
}

void Serializator::jump(int bytes)
{
	position += bytes;
}

Serializator& Serializator::operator<<( int x )
{
	int type_size = sizeof(int);
	if ( size + type_size > capacity )
	{
		capacity += BUFFER_ADD;
		buffer = (char*) realloc( buffer, capacity );
	}
	memcpy( buffer + size, &x, type_size );
	size += type_size;

	return *this;
}

Serializator& Serializator::operator>>( int &x )
{
	int type_size = sizeof(int);
	memcpy( &x, buffer + position, type_size );
	position += type_size;

	return *this;
}

Serializator& Serializator::operator<<( char x )
{
	int type_size = sizeof(char);
	if ( size + type_size > capacity )
	{
		capacity += BUFFER_ADD;
		buffer = (char*) realloc( buffer, capacity );
	}
	memcpy( buffer + size, &x, type_size );
	size += type_size;

	return *this;
}

Serializator& Serializator::operator>>( char &x )
{
	int type_size = sizeof(char);
	memcpy( &x, buffer + position, type_size );
	position += type_size;

	return *this;
}

Serializator& Serializator::operator<<( char *x )
{
	int type_size = strlen(x) + 1;	/* also copy ending zero */
	if ( size + type_size > capacity )
	{
		capacity += BUFFER_ADD;
		buffer = (char*) realloc( buffer, capacity );
	}
	memcpy( buffer + size, x, type_size );
	size += type_size;

	return *this;
}

Serializator& Serializator::operator>>( char *&x )
{
	int type_size = strlen(buffer + position) + 1;
	memcpy( x, buffer + position, type_size );
	position += type_size;

	return *this;
}

void Serializator::putBytes(char *data, int len)
{
	if ( len > BUFFER_ADD ) return;

	int type_size = len;
	if ( size + type_size > capacity )
	{
		capacity += BUFFER_ADD;
		buffer = (char*) realloc( buffer, capacity );
	}
	memcpy( buffer + size, data, type_size );
	size += type_size;
}

void Serializator::getBytes(char *data, int len)
{
	int type_size = len;
	memcpy( data, buffer + position, type_size );
	position += type_size;
}

char *Serializator::getBytesPtr(int len)
{
	char *data = buffer + position;
	position += len;
	return data;
}
