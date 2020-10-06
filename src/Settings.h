
/***************************************************************************************************
*
* SUBJECT:
*    A Benckmark for Massive Multiplayer Online Games
*    Game Server and Client
*
* AUTHOR:
*    Mihai Paslariu
*    Politehnica University of Bucharest, Romania
*    mihplaesu@yahoo.com
*
* TIME AND PLACE:
*    University of Toronto, Toronto, Canada
*    March - August 2007
*
***************************************************************************************************/

#ifndef __SETTINGS_H
#define __SETTINGS_H

/* don't monitor trasnfer: faster transfer but no statistics*/
//#define __DISABLE_RATE_MONITOR__

/* update messages from server to clients are compressed with zlib */
/* ( if this is enabled, zlib must also be linked: -lz ) */
//#define __COMPRESSED_MESSAGES__


/* each thread stores messages in a private buffer and periodicaly transfers them to the main queue */
//#define __MESSAGE_BUFFER__

/* each server has a coefficient depending on its speed (not very stable) */
//#define __SERVER_PLAYER_RATIO__

/* use 3DS models for client GUI instead of VRML models */
#define __USE_3DS_GRAPHICS__

#endif
