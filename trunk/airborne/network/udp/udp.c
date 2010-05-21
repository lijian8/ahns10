/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/udp.c,v $
 * $Log: udp.c,v $
 * Revision 1.1.1.1  2008-05-05 07:07:57  luis
 * initial import
 *
 * Revision 1.1.1.1  2008-05-05 06:49:35  luis
 * initial import
 *
 * Revision 1.1.1.1  2005/11/06 10:56:35  cvs
 * initial creeation
 *
 * Revision 1.1.1.1  2005/10/22 16:44:21  cvs
 * initial creation
 *
 * Revision 1.1.1.1  2005/10/15 15:14:06  cvs
 * initial project comments
 *
 * Revision 1.1.1.1  2004/03/03 11:03:06  srik
 * Initial Release 0.1
 *
 */
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "udp.h"

int udp_serve(int port)
{
	int s;
	struct sockaddr_in addr;

	s = socket(AF_INET,SOCK_DGRAM, 0);

	if( s < 0 )
		return -1;

	addr.sin_family		= PF_INET;
	addr.sin_port		= htons( port );
	addr.sin_addr.s_addr	= INADDR_ANY;

	if( bind( s, (struct sockaddr*) &addr, sizeof(addr) ) < 0 ){
		return -1;
	}

	return s;
}

int udp_poll(int fd,int usec)
{
	int rc;
	fd_set fds;

	struct timeval tv;

	tv.tv_usec = usec;
	tv.tv_sec = usec / 1000000;

	FD_ZERO( &fds );
	FD_SET( fd, &fds );

	rc = select(fd + 1, &fds, 0, 0, usec < 0 ? 0 : &tv);

	if( rc > 0 )
		return rc;
	if( rc == 0 )
		return 0;
	if( errno == EINTR )
		return 0;

	/* Really an error... */
	return -1;
}
int
udp_read(int fd,Host_t *addr,void *buf,int max_len)
{
	int rc;
	/* Should be socklen_t, but Darwin is borken */
	socklen_t addr_len = sizeof( *addr );

	rc = recvfrom(fd, buf, max_len, 0, (struct sockaddr*) addr,&addr_len);

	if( rc < 0 )
		return -1;

	return rc;
}

int udp_send(int fd,const Host_t *host,uint32_t type,const void *buf,int len)
{	struct timeval now;
	gettimeofday( &now, 0 );

	/* Convert time stamp to network byte order */
	now.tv_sec	= htonl( now.tv_sec );
	now.tv_usec	= htonl( now.tv_usec );
	type		= htonl( type );

	return udp_send_raw( fd, host, type, &now, buf, len );
}


int udp_send_raw(int fd,const Host_t *host,uint32_t type,const struct timeval *now,const void *buf,int len)
{
	struct iovec		vec[3];
	struct msghdr		hdr;

	vec[0].iov_base		= (void*) now;
	vec[0].iov_len		= sizeof(*now);

	vec[1].iov_base		= (void*) &type;
	vec[1].iov_len		= sizeof(type);

	vec[2].iov_base		= (void*) buf;
	vec[2].iov_len		= len;

	hdr.msg_name		= (void*) host;
	hdr.msg_namelen		= sizeof( *host );
	hdr.msg_iov		= vec;
	hdr.msg_iovlen		= 3;
	hdr.msg_control		= 0;
	hdr.msg_controllen	= 0;
	hdr.msg_flags		= 0;

	return sendmsg( fd, &hdr, 0 );
}


int udp_self(int fd,Host_t *self)
{
	/* Should be socklen_t, but Darwin is borken */
	int unused_len = sizeof(*self);

	return getsockname(fd,(struct sockaddr*) self,&unused_len);
}


char *udp_parse(char *buf,struct timeval **when, uint32_t *type)
{
	if( when )
	{
		struct timeval *ts = (struct timeval*) buf;
		*when = ts;
		ts->tv_sec = ntohl( ts->tv_sec );
		ts->tv_usec = ntohl( ts->tv_usec );
	}

	buf += sizeof( struct timeval );

	if( type ){
		 *type = ntohl( *(uint32_t*) buf );
	}

	buf += sizeof( uint32_t );

	return buf;
}



