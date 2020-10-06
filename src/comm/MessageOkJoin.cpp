
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
#include "MessageOkJoin.h" 

MessageOkJoin::MessageOkJoin(int target) : Message(MESSAGE_SC_OK_JOIN, target)
{
	name[0] = 0;
	x = y = 0;
	mapx = mapy = 0;
}

MessageOkJoin::MessageOkJoin(int target, IPaddress addr, char *name, Vector2D pos, Vector2D map) : 
																			Message(MESSAGE_SC_OK_JOIN, target, addr)
{
	strncpy(this->name, name, MAX_PLAYER_NAME);
	x = pos.x;
	y = pos.y;
	mapx = map.x; 
	mapy = map.y;
}

MessageOkJoin::MessageOkJoin(UDPpacket *p) : Message(MESSAGE_SC_OK_JOIN, 0)
{
	if ( p == NULL ) return;

	/* get basic info */
	packet = p;
	ip = packet->address.host;
	port = packet->address.port;

	/* get MessageOkJoin specific information */
	message_type = ((int*)packet->data)[0];
    message_target = ((int*)packet->data)[1];
	mapx	= ((int*)packet->data)[2];
	mapy	= ((int*)packet->data)[3];
	x	= ((int*)packet->data)[4];
	y	= ((int*)packet->data)[5];
	strncpy(name, (char*)packet->data + 6 * sizeof(int), MAX_PLAYER_NAME);
}

void MessageOkJoin::setParam(char *name, int x, int y)
{
	strncpy(this->name, name, MAX_PLAYER_NAME);
	this->x = x;
	this->y = y;
}

void MessageOkJoin::setMapSize(int x, int y)
{
	mapx = x;
	mapy = y;
}

UDPpacket *MessageOkJoin::getUDPpacket()
{
	/* packet already created */
	if ( packet != NULL ) return packet;

	/* alocate space for a new packet */
	packet = SDLNet_AllocPacket(MAX_UDP_PACKET_SIZE);
	if ( packet == NULL ) return NULL;

	/* copy data to new packet */
	packet->len = 6 * sizeof(int) + MAX_PLAYER_NAME;
	packet->address.host = ip;
	packet->address.port = port;
	((int*)packet->data)[0] = message_type;
	((int*)packet->data)[1] = message_target;
    ((int*)packet->data)[2] = mapx;
	((int*)packet->data)[3] = mapy;
	((int*)packet->data)[4] = x;
	((int*)packet->data)[5] = y;
	memcpy((char*)packet->data + 6 * sizeof(int), name, MAX_PLAYER_NAME);

	return packet;
}
