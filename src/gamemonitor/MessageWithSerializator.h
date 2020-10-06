
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

#ifndef __MESSAGE_WITH_SERIALIZATOR_H
#define __MESSAGE_WITH_SERIALIZATOR_H

#include "../comm/Message.h"

class MessageWithSerializator : public Message
{
private:
	Serializator *s;

public:
	MessageWithSerializator(int t, int target);
	MessageWithSerializator(UDPpacket *p);
	~MessageWithSerializator();

	void prepare();
	UDPpacket *getUDPpacket();
	Serializator *getSerializator();
	void setSerializator(Serializator *s);
};

#endif
