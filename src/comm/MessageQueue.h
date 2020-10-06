
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

#ifndef __MESSAGEQUEUE_H
#define __MESSAGEQUEUE_H

#include <list>
#include "Message.h"

/***************************************************************************************************
*
* Thread-safe queue for storing messages
* - This class uses a dynamic limit. First the upper size of the queue is 1. Each time the
*   consumer empties the queue, the limit is increased.
*
***************************************************************************************************/

class MessageQueue
{
private:
	list<Message*> *q;		/* the main queue for storing messages */
	SDL_mutex *qmutex;		/* mutex for synchronizing access to the queue */
	SDL_cond *qcond_low;	/* condition variable for when the queue is empty */
	SDL_cond *qcond_high;	/* condition variable for when the queue is full */
	Uint32 limit;			/* the maximum size of the queue */
	bool grow_flag;

public:
	/* Constructor / Destructor */
	MessageQueue();
	~MessageQueue();

	/* Add/Remove messages from the queue */
	void putMessage(Message *m);
	void putMessages(list<Message*> *lm);
	void putPriorityMessage(Message *m);
	
	Message *getMessage();
	Message *getMessage(Uint32 timeout);
	Message *getMessageAsync();
	
	void getMessages(list<Message*> *lm);
	void getMessages(Uint32 timeout, list<Message*> *lm);
	void getMessagesAsync(list<Message*> *lm);
	
	
	
	int size();

	/* Limit related methods */
	void setLimit(Uint32 limit);
	Uint32 getLimit();
	void setUnlimited();
};

#endif
