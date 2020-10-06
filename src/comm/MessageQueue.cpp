
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
#include "MessageQueue.h"

/***************************************************************************************************
*
* Constructor / Destructor
*
***************************************************************************************************/

MessageQueue::MessageQueue()
{
	q = new list<Message*>;
	qcond_high = SDL_CreateCond();
	qcond_low = SDL_CreateCond();
	qmutex = SDL_CreateMutex();
	if ( q == NULL || qcond_high == NULL || qcond_low == NULL || qmutex == NULL )
		throw "MessageQueue constructor failed (not enough memory)";
	limit = 1;
	grow_flag = false;
}

MessageQueue::~MessageQueue()
{
	delete q;
	SDL_DestroyMutex(qmutex);
	SDL_DestroyCond(qcond_high);
	SDL_DestroyCond(qcond_low);
}

/***************************************************************************************************
*
* Add / Remove from queue
*
***************************************************************************************************/

void MessageQueue::putMessage(Message *m)
{
	assert(m);
	
	SDL_LockMutex(qmutex);
	if ( limit != 0 && q->size() >= limit )
	{
		if ( grow_flag && limit < 0xFFFFFFFF ) grow_flag = false, limit++;
		while ( q->size() >= limit ) SDL_CondWait(qcond_high, qmutex);
	}
	q->insert(q->end(),m);
	SDL_CondSignal(qcond_low);
	SDL_UnlockMutex(qmutex);
}

void MessageQueue::putMessages(list<Message*> *lm)
{
	if ( lm == NULL ) return;

	SDL_LockMutex(qmutex);
	if ( limit != 0 && q->size() >= limit )
	{
		if ( grow_flag && limit < 0xFFFFFFFF ) grow_flag = false, limit++;
		while ( q->size() >= limit ) SDL_CondWait(qcond_high, qmutex);
	}
	q->insert(q->end(),lm->begin(),lm->end());
	SDL_CondSignal(qcond_low);
	SDL_UnlockMutex(qmutex);
}

void MessageQueue::putPriorityMessage(Message *m)
{
	SDL_LockMutex(qmutex);
	if ( limit != 0 && q->size() >= limit )
	{
		if ( grow_flag && limit < 0xFFFFFFFF ) grow_flag = false, limit++;
		while ( q->size() >= limit ) SDL_CondWait(qcond_high, qmutex);
	}
	q->insert(q->begin(),m);
	SDL_CondSignal(qcond_low);
	SDL_UnlockMutex(qmutex);
}

Message *MessageQueue::getMessage()
{
	Message *m;

	SDL_LockMutex(qmutex);
	while ( q->empty() ) SDL_CondWait(qcond_low, qmutex);
	m = q->front();
	q->pop_front();
	if ( q->size() == 0 ) grow_flag = true;
	SDL_CondSignal(qcond_high);
	SDL_UnlockMutex(qmutex);

	return m;
}

Message *MessageQueue::getMessage(Uint32 timeout)
{
	Message *m;

	SDL_LockMutex(qmutex);
	while ( q->empty() )
		if ( SDL_CondWaitTimeout(qcond_low, qmutex, timeout) != 0 )
		{
			SDL_UnlockMutex(qmutex);
			return NULL;
		}
	m = q->front();
	q->pop_front();
	if ( q->size() == 0 ) grow_flag = true;
	SDL_CondSignal(qcond_high);
	SDL_UnlockMutex(qmutex);

	return m;
}

Message *MessageQueue::getMessageAsync()
{
	Message *m = NULL;

	SDL_LockMutex(qmutex);
	if ( !q->empty() )
	{
		m = q->front();
		q->pop_front();
		if ( q->size() == 0 ) grow_flag = true;
		SDL_CondSignal(qcond_high);
	}
	SDL_UnlockMutex(qmutex);

	return m;
}

void MessageQueue::getMessages( Uint32 timeout, list<Message*> *lm)
{
	if ( lm == NULL ) return;

	SDL_LockMutex(qmutex);
	while ( q->empty() )
		if ( SDL_CondWaitTimeout(qcond_low, qmutex, timeout) != 0 )
		{
			SDL_UnlockMutex(qmutex);
			return;
		}
	while ( !q->empty() )
	{
		Message *m = q->front();
		q->pop_front();
		if ( q->size() == 0 ) grow_flag = true;
		lm->insert(lm->end(), m);
	}
	SDL_CondSignal(qcond_high);
	SDL_UnlockMutex(qmutex);
}

void MessageQueue::getMessages(list<Message*> *lm)
{
	if ( lm == NULL ) return;

	SDL_LockMutex(qmutex);
	while ( q->empty() ) SDL_CondWait(qcond_low, qmutex);
	while ( !q->empty() )
	{
		Message *m = q->front();
		q->pop_front();
		if ( q->size() == 0 ) grow_flag = true;
		lm->insert(lm->end(), m);
	}
	SDL_CondSignal(qcond_high);
	SDL_UnlockMutex(qmutex);
}

void MessageQueue::getMessagesAsync(list<Message*> *lm)
{
	if ( lm == NULL ) return;

	SDL_LockMutex(qmutex);
	while ( !q->empty() )
	{
		Message *m = q->front();
		q->pop_front();
		if ( q->size() == 0 ) grow_flag = true;
		lm->insert(lm->end(), m);
	}
	SDL_CondSignal(qcond_high);
	SDL_UnlockMutex(qmutex);
}

int MessageQueue::size()
{
	return q->size();
}

/***************************************************************************************************
*
* Limit related methods
*
***************************************************************************************************/

void MessageQueue::setLimit(Uint32 limit)
{
	this->limit = limit;
}

Uint32 MessageQueue::getLimit()
{
	return limit;
}

void MessageQueue::setUnlimited()
{
	this->limit = 0xFFFFFFFF;
}
