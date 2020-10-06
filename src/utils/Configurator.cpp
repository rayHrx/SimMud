
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

#include "General.h"
#include "Configurator.h"

Configurator::Configurator()
{
}

Configurator::Configurator(char *ini_file)
{
	addFile(ini_file);
}

bool Configurator::addFile(char *ini_file)
/* parses configuration files */
{
	FILE *f;
	int line = 0;
	char buffer[MAX_FILE_READ_BUFFER],*name,*value,*p;

	/* don't load */
	if ( ini_file == NULL ) return false;

	/* open file for reading */
	f = fopen(ini_file, "r");
	if ( f == NULL )
	{
		printf("[WARNING]Cannot open configuration file %s\n", ini_file);
		return false;
	}

	/* parse file contents */
	while ( fgets(buffer, MAX_FILE_READ_BUFFER, f) != NULL )
	{
		line++;

		/* skip empty lines, comments and titles */
		if ( buffer[0] == 0 ) break;
		if ( buffer[0] == 10 || buffer[0] == 13 ) continue;
		if ( buffer[0] == '#' || buffer[0] == '[' ) continue;

		/* locate equal sign and split the value and name */
		name = buffer;
		value = strchr(buffer, '=');
		if ( value == NULL )
		{
			printf("[WARNING]Error parsing file '%s' line %d (no equal sign)\n", ini_file, line);
			continue;
		}
		*value = 0;
		value++;

		/* remove spaces from the begining */
		for ( ; *value != 0; value++ )
			if ( !isspace(*value) ) break;
		if ( *value == 0 )
		{
			printf("[WARNING]Error parsing file '%s' line %d (no value)\n", ini_file, line);
			continue;
		}
		for ( ; *name != 0; name++ )
			if ( !isspace(*name) ) break;
		if ( *name == 0 )
		{
			printf("[WARNING]Error parsing file '%s' line %d (no value)\n", ini_file, line);
			continue;
		}

		/* remove space from the end */
		for ( p = value ; *p != 0; p++ )
			if ( isspace(*p) ) break;
		*p = 0;
		for ( p = name ; *p != 0; p++ )
			if ( isspace(*p) ) break;
		*p = 0;

		/* DEBUG */
		/* printf("'%s' -> '%s'\n", name, value); */

		/* duplicate name and value */
		name = strdup(name);
		value = strdup(value);
		if ( name == NULL || value == NULL )
		{
			printf("[WARNING]Error parsing file '%s' line %d (not enough memory)\n", ini_file, line);
			continue;
		}

		/* add contents to map */
		env[name] = value;
	}

	/* close file */
	fclose(f);
	return true;
}

Configurator::~Configurator()
{
	env.clear();
}

void Configurator::add(char *name, char *value)
{
	char *name2 = strdup(name);
	char *value2 = strdup(value);
	if ( name2 == NULL || value2 == NULL )
		throw "Configurator::add (Not enough memory)";
	env[name2] = value2;
}

void Configurator::addInt(char *name, int value)
{
	char *name2 = strdup(name);
	char *value2 = (char*)malloc( 64 );
	if ( name2 == NULL || value2 == NULL )
		throw "Configurator::add (Not enough memory)";
	sprintf(value2, "%d", value);
	env[name2] = value2;
}

char *Configurator::getAttribute(const char *name)
{
	return env[name];
}

int Configurator::getIntAttribute(const char *name)
{
	int x = 0;
	char *value;

	value = env[name];
	if ( value != NULL ) sscanf(value, "%d", &x);
	return x;
}

float Configurator::getFloatAttribute(const char *name)
{
	float x = 0;
	char *value;

	value = env[name];
	if ( value != NULL ) sscanf(value, "%f", &x);
	return x;
}
