#ifndef MESSAGEMODULE_H_
#define MESSAGEMODULE_H_


#include "MessageQueue.h"
#include "../utils/RateMonitor.h"

class Channel
{
public:
	UDPsocket 			sock;
	SDLNet_SocketSet	sset;
	UDPpacket*			pck;
	
	RateMonitor			rm[2];
	MessageQueue		m_queue[2];		//  0 == messages received
	int send_ready;						//  1 == messages sent
		
	void init(int port, SDLNet_SocketSet f_sset);
	void deinit();
};

class MessageModule : public Module
{
protected:
	volatile bool finished;
	
	int n_chans;
	Channel* chans;
	SDLNet_SocketSet full_sset;
	int thread_on;

	SDL_mutex* send_mutex;
	SDL_cond*  send_cond;
	int send_chan_id;

public:
	/* constructors/destructors */
	MessageModule(int port = 0, int _n_chans = 1, int _thread_on = 1);
	~MessageModule();
	
	void run();
	
	Message* receive( int timeout, int chan_id = 0 );
	void receive( int timeout, list<Message*> *lm, int chan_id = 0 );
	void run_receive();
	
	void send( Message* m, int chan_id = 0 );
	void send( list<Message*> *lm, int chan_id = 0 );
	void run_send();
	
	void finish();
	
	/* statistics methods */
	float getBPS( int chan_id = 0, int direction = 0 );
	
private:
	Message* receive_message( int chan_id );
	void send_message( Message* m, int chan_id );
	
	int check_send_ready();
};





#endif /*MESSAGEMODULE_H_*/
