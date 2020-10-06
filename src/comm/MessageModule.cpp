#include "General.h"
#include "MessageModule.h"

void Channel::init(int port, SDLNet_SocketSet f_sset)
{
	sset = SDLNet_AllocSocketSet(1);
	assert( sock = SDLNet_UDP_Open( port ) );
	assert( SDLNet_UDP_AddSocket(  sset, sock) != -1 );
	assert( SDLNet_UDP_AddSocket(f_sset, sock) != -1 );
	pck = NULL;
	send_ready = 0;
}

void Channel::deinit()
{
	SDLNet_UDP_Close( sock );
	if ( pck != NULL ) SDLNet_FreePacket(pck), pck = NULL;
}


MessageModule::MessageModule(int port , int _n_chans , int _thread_on )
{
	int i;
	finished = false;
	thread_on = _thread_on;
	
	assert( _n_chans > 0 && port >= 0 );
	
	// if port == 0 create only one client channel
	n_chans = ( port == 0 ) ? 1 : _n_chans;
	chans = new Channel[ n_chans ];			assert(chans);
	full_sset = SDLNet_AllocSocketSet(n_chans);
	
	// open the first channel on the specified port, and the rest on anonymous ports
	for( i = 0; i < n_chans; i++ )			chans[i].init( (i == 0) ? port : 0, full_sset );
	
	send_cond  = SDL_CreateCond();
	send_mutex = SDL_CreateMutex();
	send_chan_id = -1;
	
	if( thread_on )
	{
		f_id = 0;
		assert( SDL_CreateThread( module_thread, (void*)this ) );
		f_id = 1;
		assert( SDL_CreateThread( module_thread, (void*)this ) );
	}
}

MessageModule::~MessageModule()
{
	int i;
	for( i = 0; i < n_chans; i++ )			chans[i].deinit();
	delete[] chans;
}


void MessageModule::run()
{
	if( f_id == 0 )	run_receive();
	if( f_id == 1 )	run_send();
}


Message* MessageModule::receive( int timeout, int chan_id )
{
	int res_cs;
	assert( timeout >= 0 && chan_id >= 0 && chan_id < n_chans );
	
	if( thread_on )		return	chans[ chan_id ].m_queue[0].getMessage( timeout );
	
	res_cs = SDLNet_CheckSockets( chans[ chan_id ].sset, timeout);	assert( res_cs != -1 );
	if( !res_cs )		return NULL;			// no_activity

	return receive_message( chan_id );
}

void MessageModule::receive( int timeout,  list<Message*> *lm, int chan_id )
{
	int res_cs;
	assert( timeout >= 0 && chan_id >= 0 && chan_id < n_chans && lm );
	
	if( thread_on ){	chans[ chan_id ].m_queue[0].getMessages( timeout, lm );return;		}
	
	res_cs = SDLNet_CheckSockets( chans[ chan_id ].sset, timeout);	assert( res_cs != -1 );
	if( !res_cs )		return;					// no_activity

	Message* m = receive_message( chan_id ); 
	if( m )		lm->insert(lm->end(), m);
	return;
}

void MessageModule::run_receive()
{
	int i, res_cs;
	Message *m = NULL;
	
	while ( !finished )
	{
		res_cs = SDLNet_CheckSockets( full_sset, (Uint32)UDP_TIMEOUT);	assert( res_cs != -1 );
		if( !res_cs )	continue;			// no_activity

		for( i = 0; i < n_chans; i++ )
		{
			if( !SDLNet_SocketReady( chans[i].sock ) )		continue;
		
			m = receive_message( i );
			if( m )		chans[i].m_queue[0].putMessage(m);		
		}		
	}
}

Message* MessageModule::receive_message( int chan_id )
{
	int res_ur;
	Message *m = NULL;
	UDPpacket  *p = NULL;
		
	/* make space for the packet */	
	if( chans[ chan_id ].pck == NULL ) 	chans[ chan_id ].pck = SDLNet_AllocPacket((int)MAX_UDP_PACKET_SIZE);	
	assert( chans[ chan_id ].pck );
	p = chans[ chan_id ].pck;
	
	res_ur = SDLNet_UDP_Recv( chans[ chan_id ].sock, p );
	if ( res_ur <  0 ){		printf("[WARNING] Error receiving UDP packet");return NULL;					}
	if ( res_ur == 0 ){		printf("[WARNING] No UDP packet received on an active port");return NULL;	}
	
	/* update statistics */
	chans[ chan_id ].rm[0].addValue( p->len );
	
	/* get the user defined type from the package */
	int mtype = getPacketType( p );
			
	switch ( mtype )
	{
		case MESSAGE_SC_OK_JOIN:		m = new MessageOkJoin( p );				break;
		case MESSAGE_SC_REGULAR_UPDATE:	m = new MessageWithSerializator( p );	break;
		case MESSAGE_SC_NEW_QUEST:		m = new MessageXY( p );					break;
		default:						m = new Message( p );
	}

	chans[ chan_id ].pck = NULL;
	return m;
}


void MessageModule::send( Message* m, int chan_id )
{
	assert( chan_id >= 0 && chan_id < n_chans && m );
	
	if( !thread_on ){	send_message( m, chan_id );	return;	}
	
	chans[ chan_id ].m_queue[1].putMessage( m );
	
	SDL_LockMutex( send_mutex );
	chans[ chan_id ].send_ready = 1;
	SDL_CondSignal( send_cond );
	SDL_UnlockMutex( send_mutex );
}

void MessageModule::send( list<Message*> *lm, int chan_id )
{
	assert( chan_id >= 0 && chan_id < n_chans && lm );
	
	if( !thread_on )
	{
		while ( !lm->empty() )
		{
			send_message( lm->front(), chan_id );
			lm->pop_front();
		}
		return;	
	}
	
	chans[ chan_id ].m_queue[1].putMessages( lm );
	
	SDL_LockMutex( send_mutex );
	chans[ chan_id ].send_ready = 1;
	SDL_CondSignal( send_cond );
	SDL_UnlockMutex( send_mutex );
}



#define SEND_TIMEOUT	1000	// in milliseconds

int MessageModule::check_send_ready()
{
	int i;
	int k = ( send_chan_id + 1 ) % n_chans; 
	send_chan_id = -1;
	
	for( i = 0; i < n_chans; i++ )
		if( chans[ (k+i) % n_chans ].send_ready )
		{
			send_chan_id = (k+i) % n_chans;
			chans[ send_chan_id ].send_ready = 0;
			break;
		}
	return send_chan_id;
}

void MessageModule::run_send()
{
	/* main loop */
	while( !finished )
	{
		SDL_LockMutex( send_mutex );		
		while( check_send_ready() == -1 )
			if( SDL_CondWaitTimeout( send_cond, send_mutex, SEND_TIMEOUT) != 0 )	break;		
		SDL_UnlockMutex( send_mutex );
		
		if( send_chan_id == -1 )	continue;
		
		#ifndef __MESSAGE_BUFFER__
		
		Message* m;
		while( (m = chans[ send_chan_id ].m_queue[1].getMessageAsync()) != NULL )		send_message( m, send_chan_id );

		#else

		list<Message*> lm;
		chans[ send_chan_id ].m_queue[1].getMessagesAsync( &lm );

		while ( !lm.empty() )
		{
			send_message( lm.front(), send_chan_id );
			lm.pop_front();
		}

		#endif
	}
}


void MessageModule::send_message( Message* m, int chan_id )
{
	UDPpacket* p = m->getUDPpacket();
	
	if( SDLNet_UDP_Send( chans[ chan_id ].sock, -1, p) == 0 )		printf("[WARNING] Error sending UDP packet\n");
	chans[ chan_id ].rm[1].addValue( p->status > 0 ? p->status : 0 );
	
	delete m;	
}



void MessageModule::finish()
{
	finished = true;
}

float MessageModule::getBPS( int chan_id, int direction)
{
	assert( chan_id >=0 && chan_id < n_chans && direction >= 0 && direction < 2 );
	return chans[ chan_id ].rm[ direction ].getAverage();
}
