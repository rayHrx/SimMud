
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

#ifndef __MESSAGE_XY_H
#define __MESSAGE_XY_H

class MessageXY : public Message
{
public:
	int x,y;

public:
	MessageXY(int t, int target);
	MessageXY(int t, int target, IPaddress adr, Vector2D loc);
	MessageXY(UDPpacket *p);

	UDPpacket *getUDPpacket();
};

#endif

