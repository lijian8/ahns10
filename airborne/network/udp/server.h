/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/server.h,v $
 * $Log: server.h,v $
 * Revision 1.1.1.1  2008-05-05 07:07:57  luis
 * initial import
 *
 * Revision 1.1.1.1  2008-05-05 06:49:35  luis
 * initial import
 *
 * Revision 1.1.1.1  2005/11/06 10:56:35  cvs
 * initial creeation
 *
 * Revision 1.2  2005/10/23 17:24:28  cvs
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2005/10/22 16:44:22  cvs
 * initial creation
 *
 * Revision 1.2  2005/10/17 13:49:39  cvs
 * revision added timer for the heli reading thread
 *
 * Revision 1.1.1.1  2005/10/15 15:14:06  cvs
 * initial project comments
 *
 * Revision 1.1.1.1  2004/03/03 11:03:06  srik
 * Initial Release 0.1
 *
 */
#ifndef _SERVER_H_
#define _SERVER_H_

#include "state.h"
#include "commands.h"
#include "udp.h"


/**
 *	Max number of clients to store in the 'Host_t client[]' global array defined in server.c
 *	TODO! Remove the need to have a global client list
 */
#define	SERVER_MAX_CLIENTS 20


/**
 *	function signature for message-handlers.  All message-handlers must conform to this function signature.
 *	
 *	All message-handlers are expected to:
 *	1.	Verify the integrety of the message, making sure it is of the
 *		correct length and has not expired or is deemed to be out of sync.
 *	2.	Unpack the payload of the message and load it into the data-store 
 *		appropriately, making sure everything is in host byte order.
 *
 *	@typedef
 *
 *	@param	priv	void*				pointer to the data-store
 *	@param	src		Host_t*				defines the source of the message (program/computer that sent it)
 *	@param	type	int					the message type which was encoded into the message packet. ie: HELI_STATE, IMODE_STATE.  This is only important if a message-handler can handle more than one message type
 *	@param	when	struct timeval*		the timestamp which was encoded into the message packet.  There is no guarantee that this wasn't faked
 *	@param	data	void*				pointer to the payload of the message
 *	@param	len		size_t				length of the payload in bytes
 *
 *	@return			int					true (non-zero) if the message was accepted and processed, false (zero) if the message was dismissed for whatever reason
 */



typedef int (*handler_t)(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t	len);

typedef struct {
	handler_t func;
	void *data;
}handler;
typedef struct {
	int sock;
	Host_t server;
	struct timeval last_packet;
	handler handlers[256];
}Server;


void server_init(Server *server, int port);
int server_process_state(void *priv,const Host_t *src, int type, const struct timeval *when, const void *data, size_t len);

void server_update(int fd,void *voidp);
int server_get_packet(Server *server);
void server_add_client(Server *server, const Host_t *src);
void server_del_client(Server *server, const Host_t *src);
int server_poll(Server *server, int usec);


/**
 *	Registers a message-handler function and data-store to be used for processing incoming messages of the specified type
 *
 *	All message-handler functions are responsible for unpacking the payload of 
 *	the message and loading it into the data-store appropriately, making sure 
 *	everything is in host byte order.  They should be responsible for dismissing
 *	messages that are deemed to have expired or are out of sync.
 *	
 *	This minimum requirement of a message-handler should be provided as a default
 *	implementation (function specific to the message type, such as server_cam_state(),
 *	server_heliGPS_state(), etc) within this module.  If a user of this server module
 *	would like override the default message-handler for a particular message, they 
 *	should make sure their custom message-handler calls the related default one.
 *	This should usually be the first step in the custom message-handler.
 *	
 *	@param	server	Server*		the server/socket that the handler relates to
 *	@param	type	int			the message type that the handler relates to. ie. HELI_STATE, IMODE_STATE
 *	@param	func	handler_t	the handler function to use
 *	@param	data	void*		pointer to the data-store associated with this message type/server combination.
 *								The default handler should populate this data-store appropriately
 *	@return	void
 */



void server_handle(Server *server, int type, handler_t func,void *data);

void server_connect(Server *server, const char *hostname,int port);
void server_send_command(Server *server, int type);
void server_send_parameter( Server *server,int type, double value);

int server_cam_state(void *priv,const Host_t *src, int type, const struct timeval *when, const void *data, size_t len);
int server_heliAutopilot_state(void *priv,const Host_t *src, int type, const struct timeval *when, const void *data, size_t len);
int server_heliGPS_state(void *priv,const Host_t *src, int type, const struct timeval *when, const void *data, size_t len);
int server_flyto_state(void *priv,const Host_t *src, int type, const struct timeval *when, const void *data, size_t len);


/**
 *	Default message handler for imode_state messages (the messages the iphone sends)
 *	
 *	All message-handlers are expected to:
 *	1.	Verify the integrety of the message, making sure it is of the
 *		correct length and has not expired or is deemed to be out of sync.
 *	2.	Unpack the payload of the message and load it into the data-store (priv)
 *		appropriately, making sure everything is in host byte order.
 *
 *	@param	priv	void*				pointer to the iphone_primary_input_sample_t data-store 
 *										which, on successful completion, loaded with the payload 
 *										of the message
 *	@param	src		Host_t*				defines the source of the message (program/computer that sent it)
 *	@param	type	int					the message type which was encoded into the message 
 *										packet. It should be IMODE_STATE.
 *	@param	when	struct timeval*		the timestamp which was encoded into the message packet.  
 *										There is no guarantee that this wasn't faked
 *	@param	data	void*				pointer to the payload of the message
 *	@param	len		size_t				length of the payload in bytes
 *
 *	@return			int					true (non-zero) if the message was accepted and processed, false (zero) if the message was dismissed for whatever reason
 */
int server_default_message_handler_imode_state(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len);




/*
 * Send a packet to all clients.
 */
void server_send_packet(Server *server, int type,const void *buf, size_t len);
#endif

