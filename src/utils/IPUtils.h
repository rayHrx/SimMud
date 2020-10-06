
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

#ifndef __IP_UTILS_H
#define __IP_UTILS_H

#include "../General.h"


/* Ip comparators */
/***********************************************************************/

#define ipcmp(ip1,ip2) (( ip1.host != ip2.host ) ? \
	( ip1.host - ip2.host ) \
	: ( ip1.port - ip2.port ))

#define equalIP(a1,a2) (a1.host == a2.host && a1.port == a2.port)

struct IpComparator
{
	bool operator() ( IPaddress a1, IPaddress a2 ) const
	{
		return ( (int)ipcmp(a1,a2) < 0 );
	}
};

/* Functions */
/***********************************************************************/

/* Get IP for name and port ( returns true if successfull ) */
bool resolveHost(IPaddress *addr, char *host_name);

inline void printAddress(IPaddress a)
{
	unsigned short port;
	unsigned char b1,b2,b3,b4;
	port = ( ( a.port & 0xFF ) << 8 ) | ( a.port >> 8 );
	b1 = a.host & 0xFF;
	b2 = ( a.host >> 8 )& 0xFF;
	b3 = ( a.host >> 16 )& 0xFF;
	b4 = ( a.host >> 24 )& 0xFF;

	printf("%u.%u.%u.%u:%d", b1,b2,b3,b4, port);
}

#endif
