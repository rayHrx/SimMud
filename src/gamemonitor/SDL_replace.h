
/***************************************************************************************************
*
* SUBJECT:
*    A monitoring application for the Massive Multiplayer Online Game benchmark
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

/***************************************************************************************************
*
* This file defines some types from SDL and SDL_net, used in applications that don't inclunde SDL.h
* but need those types
*
***************************************************************************************************/

#ifndef __SDL_REPLACE
#define __SDL_REPLACE

#define Uint8 unsigned char
#define Uint16 unsigned short
#define Uint32 unsigned int
#define Sint8 char
#define Sint16 short
#define Sint32 int

typedef struct {
    Uint32 host;            /* 32-bit IPv4 host address */
    Uint16 port;            /* 16-bit protocol port */
} IPaddress;

#define TCPsocket void*
#define UDPsocket void*

#endif
