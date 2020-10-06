#ifndef REGION_H_
#define REGION_H_

#include <list>
using namespace std;

#include "../General.h"

#include "Player.h"
#include "GameObject.h"

typedef struct
{
	Vector2D pos;			// coordinates of the upper left corner of the region
	Vector2D size;			// size of the region
	
	int layout;				// = thread_id of the thread handling the players from this region
	
	list<Player*> players;
	int n_pls;
	
	list<GameObject*> objects;	
	
	SDL_mutex *mutex;
} Region;

void initRegion( Region* r, Vector2D p, Vector2D sz, int _layout, list<GameObject*> objs, list<Player*> pls );

int 		Region_addPlayer( Region* r, Player* p );
void		Region_removePlayer( Region* r, Player* p );
bool		Region_movePlayer( Region* r_old,  Region* r_new, Player* p, Vector2D n_pos );
Player*		Region_getPlayer( Region* r, Vector2D loc );

int			Region_addObject( Region* r, GameObject *o, int min_res, int max_res );
GameObject*	Region_getObject( Region* r, Vector2D loc );

void		Region_rewardPlayers( Region* r, int bonus, int max_life );
void 		Region_regenerateObjects( Region* r, int max_res );

#endif /*REGION_H_*/
