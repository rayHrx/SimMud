
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

#include <General.h>
#include "RateMonitor.h"

/***************************************************************************************************
*
* Constructor
*
***************************************************************************************************/

RateMonitor::RateMonitor()
{
	n = 0;
	Uint32 ts = SDL_GetTicks();
	for ( int i = 0; i < HOLD_VALUES; i++ )
	{
		timestamps[i] = ts;
		values[i] = 0;
	}
}

/***************************************************************************************************
*
* addValue
* - Adds a value to the samples vector and its timestamp to the timestamps vector
*
***************************************************************************************************/

void RateMonitor::addValue(int x)
{
#ifndef __DISABLE_RATE_MONITOR__
	values[n] = x;
	timestamps[n] = SDL_GetTicks();
	n = ( n + 1 ) % HOLD_VALUES;
#endif
}

/***************************************************************************************************
*
* getAverage
* - Computes the average of all values hold in the vector after discarding old values.
*
***************************************************************************************************/

float RateMonitor::getAverage()
{
#ifndef __DISABLE_RATE_MONITOR__
	int i,sum;

	/* get current time */
	Uint32 now = SDL_GetTicks();

	/* invalidate old records */
	for ( i = 0; i < HOLD_VALUES; i++ )
	{
		if ( now - timestamps[n] > 7000 )
		{
			timestamps[n] = now;
			values[n] = 0;
		} else break;
		n = ( n + 1 ) % HOLD_VALUES;
	}

	/* get interval */
	Uint32 interval = now - timestamps[ (n+1) % HOLD_VALUES ];
	if ( interval == 0 ) return 0;	/* interval is 0 wnen there is no data */

	/* get bytes sent in this interval */
	sum = 0;
	for ( i = 0; i < HOLD_VALUES; i++ )
		sum += values[i], values[i] = 0, timestamps[i] = now;
	n = 0;

	/* return answer */
	return (float) sum * 1000.0 / (float) interval;
#else
	return 0;
#endif
}
