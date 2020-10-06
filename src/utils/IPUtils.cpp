
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
#include "IPUtils.h"

bool resolveHost(IPaddress *addr, char *host_name)
{
	int port;
	bool result;

	if ( addr == NULL || host_name == NULL ) return false;

	/* separate port from name */
	char *ptr = strchr(host_name, ':');
	if ( ptr != NULL )
	{
		*ptr = 0;
		sscanf(ptr+1, "%d", &port);
	} else port = 0;

	/* call DNS */
	result = true;
	if ( SDLNet_ResolveHost(addr, host_name, port) ) result = false;

	/* restore original string */
	if ( ptr != NULL ) *ptr = ':';

	return result;
}
