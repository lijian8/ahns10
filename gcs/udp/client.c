/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/client.c,v $
 * $Log: client.c,v $
 * Revision 1.1.1.1  2008-05-05 07:07:57  luis
 * initial import
 *
 * Revision 1.1.1.1  2008-05-05 06:49:35  luis
 * initial import
 *
 * Revision 1.1.1.1  2005/11/06 10:56:35  cvs
 * initial creeation
 *
 * Revision 1.1.1.1  2005/10/22 16:44:22  cvs
 * initial creation
 *
 * Revision 1.1.1.1  2005/10/15 15:14:06  cvs
 * initial project comments
 *
 * Revision 1.1.1.1  2004/03/03 11:03:06  srik
 * Initial Release 0.1
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "state.h"
#include "server.h"
#include "udp.h"
#include "commands.h"

static int		sock;
static Host_t   server;
int
host_lookup(struct sockaddr_in *server,const char *hostname,int port)
{
	struct hostent *	hp;

	
	if( !(hp = gethostbyname(hostname)) )
		return -1;

	server->sin_family	= hp->h_addrtype;
	server->sin_port	= htons( port );
	memcpy( &server->sin_addr, hp->h_addr, hp->h_length );

	return 0;
}


int connect_state(Server *server, const char *hostname,int port)
{
	fprintf(stderr, "Deprecated connect_state called.  Please fix!\n" );

	if( !hostname )
		hostname = getenv( "SIMSERVER" );
	if( !hostname )
		hostname = "localhost";

	if( !port ){
		const char *port_env = getenv( "SIMPORT" );
		if( port_env )
			port = atoi( port_env );
	}

	if( !port )
		port = 2002;
	
	if( host_lookup( &(server->server), hostname, port ) < 0 ){
		perror( hostname );
		return -1;
	}
	if( (server->sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
		perror( "socket" );
		return -1;
	}
	if (connect(server->sock, (struct sockaddr *) &(server->server),
              sizeof(server->server)) < 0){
		fprintf(stderr, "unable to connect\n");
		exit(-1);
	}

	/* Try to connect */
	udp_send(sock, &(server->server), 1, 0, 0);

	return sock;
}
int set_parameter(int type,double value)
{
	return udp_send(sock, &server, type, &value, sizeof(value));
}


int send_command(int type)
{
	return udp_send(sock, &server, type, 0, 0);
}


int read_state(state_t *state,int forever)
{

	int frames = 5;
	int frames_read = 0;
	static struct timeval	last_packet;

	while( udp_poll( sock, forever ? -1 : 1 ) > 0 && --frames > 0 )
	{
		int len;
		char buf[ 1024 ];
		struct timeval *when;
		uint32_t type;
		const char *data;
		Host_t src;

		len = udp_read( sock, &src, buf, sizeof(buf) );
		data = udp_parse( buf, &when, &type );


		printf( "%d:%06d: Got type %d\n",
			(int)when->tv_sec,
			(int)when->tv_usec,
			type);


		if( timercmp( &last_packet, when, > ) ){
			printf( "\tOld packet\n" );
			continue;
		}

		if( type == SIM_QUIT ){
			return -1;
		}

		if( type == HELI_STATE ){
			frames_read++;
			last_packet = *when;
			memcpy( state, data, sizeof(*state) );
		}
	}

	return frames_read;
}



