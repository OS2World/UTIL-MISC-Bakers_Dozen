/*  gpib_c.h
 *  Rev: 2009 Mar 10
 */

#ifndef GPIB_C_H
#define GPIB_C_H 1

#define ERR     (1<<15)

int   gpib_init( void );
void  gpib_quit( void );

int   gpib_open( int addr );
void  gpib_close( int dev );
int   gpib_write( int dev, char* cmd );
int   gpib_read( int dev, char* buf, int size );

int   gpib_status( void );
char* gpib_error( );

#endif
