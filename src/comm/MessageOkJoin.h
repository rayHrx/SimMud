
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

#ifndef __MESSAGE_JOIN_H
#define __MESSAGE_JOIN_H

#include "General.h"
#include "Message.h"

class MessageOkJoin : public Message
{
public:
	char name[MAX_PLAYER_NAME];
	int x,y;
	int mapx,mapy;

public:
	MessageOkJoin(int target);
	MessageOkJoin(int target, IPaddress addr, char *name, Vector2D pos, Vector2D map);
	MessageOkJoin(UDPpacket *p);

	void setParam(char *name, int x, int y);
	void setMapSize(int x, int y);

	UDPpacket *getUDPpacket();
};

#endif
