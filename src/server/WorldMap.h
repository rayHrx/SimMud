#ifndef __WORLDMAP_H
#define __WORLDMAP_H

#include "../General.h"
#include "../comm/Message.h"

#include "PlayerBucket.h"
#include "../game/Region.h"

class WorldMap
{
public:
    char** terrain;
    PlayerBucket* players;	/*array of PlayerBuckets, one for each thread */
    int n_players;
    
    Region** regions;
    Vector2D n_regs;
    
    /* map and region */
	Vector2D size;					/* map dimensions */
	Vector2D regmax, regmin;		/* size of regions */

	/* region properties */
	int blocks;				/* the number of blocked cells from 1000 cells */
	int resources;			/* the number of resources on the map */
	int min_res,max_res;	/* the minimun and maximum quantity	a resource can have (min 1, max 10) */


    Uint32 last_balance;   
    

	void generate();
	
	Player* addPlayer( IPaddress a );	
	Player* findPlayer( IPaddress a, int t_id );
	
    void removePlayer(Player* p);
    void movePlayer(Player* p);
    
    void useGameObject( Player* p );
    void attackPlayer( Player* p, int attack_dir );
    void updatePlayer(Player* p, Serializator* s);
    
    Region* getRegionByLocation( Vector2D loc);
    
    void balance();
    void balance_lightest();
    void balance_spread();

    bool isOverloaded( int n_pl );
    void reassignRegion( Region* r, int new_layout );
    
    void regenerateObjects();
    void rewardPlayers( Vector2D quest_pos );
};



#endif
