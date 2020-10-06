
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

#ifndef __MESSAGE_H
#define __MESSAGE_H


/***************************************************************************************************
*
* define all message types here
*
***************************************************************************************************/

enum MessageEnum
{
	/* no topic */
	MESSAGE_DEFAULT = 0,

	/* basic client messages */
	MESSAGE_CS_JOIN,
	MESSAGE_SC_OK_JOIN,			/* needs own class */
	MESSAGE_SC_NOK_JOIN,
	MESSAGE_CS_LEAVE,
	MESSAGE_SC_OK_LEAVE,

	/* quest messages */
	MESSAGE_SC_NEW_QUEST,			/* needs own class */
	MESSAGE_SC_QUEST_OVER,

	/* player actions */
	MESSAGE_CS_MOVE_DOWN,
	MESSAGE_CS_MOVE_RIGHT,
	MESSAGE_CS_MOVE_UP,
	MESSAGE_CS_MOVE_LEFT,
	
	MESSAGE_CS_USE,
	MESSAGE_CS_ATTACK_DOWN,
	MESSAGE_CS_ATTACK_RIGHT,
	MESSAGE_CS_ATTACK_UP,
	MESSAGE_CS_ATTACK_LEFT,

	/* update game state messages */
	MESSAGE_SC_REGULAR_UPDATE,		/* needs own class */
};

/***************************************************************************************************
*
* Basic Message class
*
***************************************************************************************************/

class Message
{
public:
	int message_type;	/* user defined message type */
	int message_target; /* tid of message target */
    Uint32 ip;		/* IP address */
	Uint16 port;		/* only for sending messages */
	UDPpacket *packet;	/* received or sent packet */


	/* constructors */
	Message();
	Message(int t, int target);		/* create an empty message of the specified type */
	Message(int t, int target, IPaddress adr);
	Message(UDPpacket *p);	/* interpret a received packet */
	virtual ~Message();

	/* public methods */
	int getType();
    int getTarget();
	void setAddress(Uint32 ip, Uint16 port);
	void setAddress(IPaddress adr);
	IPaddress getAddress();
	Uint32 getIP();
	Uint16 getPort();
	virtual UDPpacket *getUDPpacket();
};

/***************************************************************************************************
*
* Other Message classes
*
***************************************************************************************************/

#include "MessageOkJoin.h"
#include "MessageWithSerializator.h"
#include "MessageXY.h"


/***************************************************************************************************
*
* Obtain message type
*
***************************************************************************************************/

/* - the first int in the data field in UPDpacket is the message type */
/* - this function doesn't check if p is not NULL */
int getPacketType(UDPpacket *p);
int getPacketTarget(UDPpacket *p);
#endif
