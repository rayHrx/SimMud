
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

#ifndef __RATE_MONITOR_H
#define __RATE_MONITOR_H

#define HOLD_VALUES 4096

/***************************************************************************************************
*
* RateMonitor - used fo bandwidth statistics
* - This class uses HOLD_VALUES samples. Each time a send/recv is called, the number of bytes
*   transfered is added to the values vector and the timestamp is added to the timestamps vector.
*   When the getAverage method is called, old values are discarded, then the average of the
*   remaining values is computed, finaly even these values are discarded.
*
***************************************************************************************************/

class RateMonitor
{
private:
	int n;	/* number of samples */
	Uint32 timestamps[HOLD_VALUES];
	int values[HOLD_VALUES];	/* values for the number of bytes in send/recv calls */

public:
	RateMonitor();		/* constructor */
	void addValue(int x);	/* add a sample to the vector */
	float getAverage();	/* computes the average of recent values in the vector */
};

#endif
