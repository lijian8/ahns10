/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/udp.h,v $
 * $Log: udp.h,v $
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
#ifndef _STATE_UDP_H_
#define _STATE_UDP_H_

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "state.h"
typedef struct sockaddr_in Host_t;

extern int udp_serve(int port);
extern int udp_poll(int fd,int usec);
extern int udp_read(int fd,Host_t *src,void *buf,int max_len);

extern int udp_send(int fd,const Host_t *dest,uint32_t type,const void *buf,int max_len);

extern int udp_send_raw(int fd,const Host_t *dest,uint32_t type,const struct timeval *timestamp,const void *buf,int max_len);

extern int udp_self(int fd,Host_t *self);
extern char *udp_parse(char *buf,struct timeval **when,uint32_t *type);

#endif

