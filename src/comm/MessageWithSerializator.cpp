
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

#ifdef __COMPRESSED_MESSAGES__
#include <zlib.h>
#endif

MessageWithSerializator::MessageWithSerializator(int t, int target, IPaddress adr) : Message(t, target, adr)
{
	/* alocate space for a new packet */
	packet = SDLNet_AllocPacket((int)MAX_UDP_PACKET_SIZE);	assert( packet );
	memset(packet->data, 0, MAX_UDP_PACKET_SIZE);
	packet->len = MAX_UDP_PACKET_SIZE;
	packet->address.host = ip;
	packet->address.port = port;

	/* create the serializator */
	#ifdef __COMPRESSED_MESSAGES__
	s = new Serializator();
	#else
	s = new Serializator((char*)packet->data, 0, packet->len);
	#endif
	if ( s != NULL )
    {
        *s << (int)message_type;
        *s << (int)message_target;
    }
}

MessageWithSerializator::MessageWithSerializator(UDPpacket *p) : Message()
{
	if ( p == NULL ) return;

	/* get basic info */
	packet = p;
	ip = packet->address.host;
	port = packet->address.port;

	/* create the serializator */
	#ifdef __COMPRESSED_MESSAGES__
	uLongf d = ((int*)(packet->data))[1];
	char *buffer = (char*)malloc( d + sizeof(int) );
	int err = uncompress((Bytef*)(buffer + sizeof(int)), &d,
		(Bytef*)((char*)packet->data + 2*sizeof(int)), packet->len - 2*sizeof(int));
	if ( err != Z_OK )
	{
		printf("[WARNING] Uncompress error %d\n", err);
		s = NULL;
		message_type = 0;
		return;
	}
	((int*)buffer)[0] = ((int*)packet->data)[0];
	s = new Serializator((char*)buffer, d + sizeof(int));
	s->can_grow = true;
	#else
	s = new Serializator((char*)packet->data, (int)packet->len);
	#endif

	if ( s != NULL )
    {
        *s >> message_type;
        *s >> message_target;
    }
}

MessageWithSerializator::~MessageWithSerializator()
{
	if ( s != NULL ) delete s;
}

void MessageWithSerializator::prepare()
{
	#ifdef __COMPRESSED_MESSAGES__
	uLongf d = MAX_UPD_PACKET_SIZE - 2 * sizeof(int);
	int err = compress((Bytef*)(((char*)packet->data) + 2 * sizeof(int)), &d,
		(Bytef*)s->getBuffer()+sizeof(int), s->getSize()-sizeof(int));
	if ( err != Z_OK )
	{
		printf("[WARNING] Message compression error %d\n", err);
		return;
	}
	((int*)packet->data)[0] = message_type;
	((int*)packet->data)[1] = s->getSize() - sizeof(int);
	packet->len = d + 2 * sizeof(int);
	#else
	if ( s != NULL ) packet->len = s->getSize();
	#endif
}

UDPpacket *MessageWithSerializator::getUDPpacket()
{
	#ifndef __COMPRESSED_MESSAGES__
	if ( s != NULL ) packet->len = s->getSize();
	#endif

	return packet;
}

Serializator *MessageWithSerializator::getSerializator()
{
	return s;
}

void MessageWithSerializator::setSerializator(Serializator *s)
{
	this->s = s;
}
