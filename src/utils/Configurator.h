
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

#ifndef __CONFIGURATOR_H
#define __CONFIGURATOR_H

#include <map>

/* String comparator */
struct StrCompare
{
	bool operator()( const char* s1, const char* s2 ) const
	{
		return strcmp( s1, s2 ) < 0;
	}
};

typedef		map< const char*, char*, StrCompare >	ConfiguratorMap;

/***************************************************************************************************
*
* Configurator - class for loading INI files
*
***************************************************************************************************/

class Configurator
{
private:
	ConfiguratorMap env;

public:
	Configurator();
	Configurator(char *ini_file);
	~Configurator();

	bool addFile(char *ini_file);
	void add(char *name, char *value);
	void addInt(char *name, int value);
	char *getAttribute(const char *name);
	int getIntAttribute(const char *name);
	float getFloatAttribute(const char *name);
};

#endif
