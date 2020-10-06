
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
#include "Message.h"

MessageXY::MessageXY(int t, int target) : Message(t, target)
{
	x = y = 0;
}

MessageXY::MessageXY(int t, int target, IPaddress adr, Vector2D loc) : Message(t, target, adr)
{
	this->x = loc.x;
	this->y = loc.y;
}

MessageXY::MessageXY(UDPpacket *p) : Message()
{
	if ( p == NULL ) return;

	/* get basic info */
	packet = p;
	ip = packet->address.host;
	port = packet->address.port;

	/* get MessageOkJoin specific information */
	message_type = ((int*)packet->data)[0];
	message_target = ((int*)packet->data)[1];
    x = ((int*)packet->data)[2];
	y = ((int*)packet->data)[3];
}


UDPpacket *MessageXY::getUDPpacket()
{
	/* packet already created */
	if ( packet != NULL ) return packet;

	/* alocate space for a new packet */
	packet = SDLNet_AllocPacket(MAX_UDP_PACKET_SIZE);
	if ( packet == NULL ) return NULL;

	/* copy data to new packet */
	packet->len = 4 * sizeof(int);
	packet->address.host = ip;
	packet->address.port = port;
	((int*)packet->data)[0] = message_type;
	((int*)packet->data)[1] = message_target;
	((int*)packet->data)[2] = x;
	((int*)packet->data)[3] = y;

	return packet;
}

