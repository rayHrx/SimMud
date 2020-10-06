
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

#ifndef __CLIENT_AI_MODULE
#define __CLIENT_AI_MODULE

#include "Client.h"
#include "PlayerAI.h"
#include "../utils/IPUtils.h"
#include "../comm/Message.h"
#include "../comm/MessageModule.h"

class ClientActionModule : public Module
{
private:
	/* data shared by all client threads */
	ClientData *client_data;

	/* object to hold AI data */
	PlayerAI *ai;

	MessageModule* comm;

	/* the number of times the client tried to contact the server
	  (max is RETRY_COUNT defined in ../general.h) */
	int connect_retry_count;
	/* the maximum number of times the client sends a leave message */
	int leave_retry_cont;
	/* the time player migration is started */
	/* (used to timeout if migration is taking too long) */
	Uint32 migration_start_time;
	/* number of migrate messages without an ok_migrate answer */
	int migration_counter;
	/* the last time the client has received a regular update */
	Uint32 time_of_last_update;

public:
	/* constructor and setup methods */
	ClientActionModule(ClientData *cd, MessageModule* _comm);

	/* main loop */
	void run();

	/* action taken depending on client state */
	void action_INITIAL();
	void action_WAITING_JOIN();
	void action_PLAYING();
	void action_WAITING_LEAVE();
	void action_MIGRATING();

	/* action taken when a message is received */
	void handle_OK_JOIN(Message *message);
	void handle_NOK_JOIN();
	void handle_OK_LEAVE();
	void handle_NEW_QUEST(Message *message);
	void handle_QUEST_OVER();
	void handle_REGULAR_UPDATE(Message *message);
	void handle_ANSWER_CLIENT_UPDATE(Message *m);
	void handle_MIGRATE(Message *m);
	void handle_OK_MIGRATE(Message *m);

	/* other methods */
	//void updateMapFromSerializator(Serializator *s);
};

#endif
