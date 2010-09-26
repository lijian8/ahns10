/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/server.c,v $
 * $Log: server.c,v $
 *
 */
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

#include "server.h"
#include "state.h"
#include "commands.h"
#include "udp.h"

#include "control.h"


/** Array of client IP addresses and Ports */
static Host_t client[SERVER_MAX_CLIENTS];


/** client Array index variable*/
static int i = 0;

static int client_added = 0;
static int client_removed = 0;

void server_handle(Server *server, int type, handler_t func,void *data) 
{
	server->handlers[type].func = func;
	server->handlers[type].data = data;
}

static int command_nop(void *priv,const Host_t *src, int type,const struct timeval *when,const void *data,size_t len)

{
	return 1;
}

void server_add_client(Server *server,const Host_t *src)
{
	unsigned char BUFFER[80];
	PackUInt32(BUFFER,COMMAND_OPEN);                       
	fprintf(stderr,"void server_add_client(Server *server,const Host_t *src)\n");
	
	// First make sure the client isn't already in the global list of clients
	int index;
	for (index = 0; index < SERVER_MAX_CLIENTS; index++)
	{
		if (client[index].sin_addr.s_addr == src->sin_addr.s_addr && client[index].sin_port == src->sin_port)
		{
			// This client is already in the array.  Send it a COMMAND_ACK and exit
			udp_send(server->sock, &client[index], COMMAND_ACK, BUFFER, sizeof(uint32_t));
			return;
		}
	}
	
	// Make sure the maximum number of clients hasn't been reached
	if (i == SERVER_MAX_CLIENTS)
	{
		// This should terminate the program
		fprintf(stderr, "ERROR: Could not add the client to the global list of clients because the max limit of %d clients has been reached.  It should be noted that there is currently no mechanisim for purging clients from this list.\n", SERVER_MAX_CLIENTS);
		exit(-1);
	}
	
	// The client isn't already in the list of clients.  Add it now.
	client[i].sin_addr.s_addr = src->sin_addr.s_addr;
	client[i].sin_port = src->sin_port;
     
   	udp_send(server->sock, &client[i], COMMAND_ACK, BUFFER, sizeof(uint32_t));

	i++;
	client_added =1;

}

void server_del_client(Server *server, const Host_t *src)
{
	fprintf(stderr,"void server_del_client(Server *server, const Host_t *src)\n");
	int j = 0;
	int k = 0;
        uint8_t found = 0;

	do // Find client in array
	{
		if ((client[j].sin_addr.s_addr == src->sin_addr.s_addr) && (client[j].sin_port == src->sin_port)) // Same Port and IP
		{
			udp_send(server->sock, &client[j], COMMAND_CLOSE, 0, 0);
			for (k = j; k < i-1; k++) // shuffle clients down when SERVER_MAX_CLIENTS > 1
			{
				client[k].sin_addr.s_addr = client[k+1].sin_addr.s_addr;
				client[k].sin_port = client[k+1].sin_port;
			}
			// no suffle possible for last element, zero it 
			client[i-1].sin_addr.s_addr = 0;
			client[i-1].sin_port = 0;
			i--;
 			found = 1;
		}
		++j;
	} while( j < i && !found);

        if (!found) // client not in array, send Close anyway
        {
		udp_send(server->sock, src, COMMAND_CLOSE, 0, 0);
        }
	client_removed = 1;
}


static int command_open(void *priv,const Host_t *src, int type,const struct timeval *when,const void *data,size_t len)
{
	Server *self = (Server*) priv;
	

	server_add_client( self, src );

	return 1;
}

static int command_close(void *priv,const Host_t *src, int type,const struct timeval *when,const void *data,size_t len)
{
	Server *self = (Server*) priv;

	server_del_client( self, src );

	return 1;
}

static int set_config(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	Server *self = (Server*) priv;
        unsigned char* BUFFER[sizeof(uint32_t)];
        PackUInt32(BUFFER,SET_CONFIG);
        
        // Unpack ap_config_t structure
        ap_config_t receivedConfig;
        UnpackAPConfigData((unsigned char*) data, &receivedConfig);

        // Verify  and Save Configuration for Control
        if(setAPConfig(&receivedConfig))
        {		         
        	// Responed with command_ack and set_config
        	udp_send(self->sock, src, COMMAND_ACK, BUFFER, sizeof(uint32_t));
     	}

        fprintf(stderr,"receivedConfig \nRoll Active = %u \tPitch Active = %u \tYaw Active = %u\n", receivedConfig.phiActive, receivedConfig.thetaActive, receivedConfig.psiActive);
        fprintf(stderr,"x Active = %u \ty Active = %u \tz Active = %u\n", receivedConfig.xActive, receivedConfig.yActive, receivedConfig.zActive);
        
	return 1;
}

static int set_gains(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	Server *self = (Server*) priv;
        unsigned char* BUFFER[sizeof(uint32_t)];
	PackUInt32(BUFFER, GAINS);

        // Unpack gains_t structure
        gains_t receivedGains;
        UnpackGains((unsigned char*) data, &receivedGains);

        // Verify  and Save Configuration for Control
        if(setGains(&receivedGains))
        {		         
        	// Responed with command_ack and set_config
        	udp_send(self->sock, src, COMMAND_ACK, BUFFER, sizeof(uint32_t));
     	}

        fprintf(stderr,"receivedGains \nRoll = %f %f %f", receivedGains.rollKp, receivedGains.rollKi, receivedGains.rollKd);
        fprintf(stderr,"\tPitch = %f %f %f", receivedGains.pitchKp, receivedGains.pitchKi, receivedGains.pitchKd);
        fprintf(stderr,"\tYaw = %f %f %f \n", receivedGains.yawKp, receivedGains.yawKi, receivedGains.yawKd);
        fprintf(stderr,"X = %f %f %f \t", receivedGains.xKp, receivedGains.xKi, receivedGains.xKd);
        fprintf(stderr,"\tY = %f %f %f ", receivedGains.yKp, receivedGains.yKi, receivedGains.yKd);
        fprintf(stderr,"\tZ = %f %f %f \n", receivedGains.zKp, receivedGains.zKi, receivedGains.zKd);
        
	return 1;
}

static int set_parameters(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	Server *self = (Server*) priv;
        unsigned char* BUFFER[sizeof(uint32_t)];
	PackUInt32(BUFFER, PARAMETERS);

        // Unpack gains_t structure
        loop_parameters_t receivedParameters;
        UnpackParameters((unsigned char*) data, &receivedParameters);

        // Verify  and Save Configuration for Control
        if(setParameters(&receivedParameters))
        {		         
        	// Responed with command_ack and set_config
        	udp_send(self->sock, src, COMMAND_ACK, BUFFER, sizeof(uint32_t));
     	}

        fprintf(stderr,"receivedParameters \nRoll = %f %f %f", receivedParameters.rollMaximum, receivedParameters.rollMinimum, receivedParameters.rollNeutral);
        fprintf(stderr,"\tPitch = %f %f %f", receivedParameters.pitchMaximum, receivedParameters.pitchMinimum, receivedParameters.pitchNeutral);
        fprintf(stderr,"\tYaw = %f %f %f \n", receivedParameters.yawMaximum, receivedParameters.yawMinimum, receivedParameters.yawNeutral);
        fprintf(stderr,"X = %f %f %f \t", receivedParameters.xMaximum, receivedParameters.xMinimum, receivedParameters.xNeutral);
        fprintf(stderr,"\tY = %f %f %f ", receivedParameters.yMaximum, receivedParameters.yMinimum, receivedParameters.yNeutral);
        fprintf(stderr,"\tZ = %f %f %f \n", receivedParameters.zMaximum, receivedParameters.zMinimum, receivedParameters.zNeutral);
	return 1;
}

static int desired_position(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	Server *self = (Server*) priv;
        unsigned char* BUFFER[sizeof(uint32_t)];
	PackUInt32(BUFFER, DESIRED_POSITION);

        // Unpack position_t structure
        position_t receivedPosition;
        UnpackPositionData((unsigned char*) data, &receivedPosition);

        // Verify  and Save Configuration for Control
        if(setPosition(&receivedPosition))
        {		         
        	// Responed with command_ack and set_config
        	udp_send(self->sock, src, COMMAND_ACK, BUFFER, sizeof(uint32_t));
     	}

        fprintf(stderr,"receivedPosition \nPosition = %f %f %f", receivedPosition.x, receivedPosition.y, receivedPosition.z);
	return 1;
}

static int desired_attitude(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	Server *self = (Server*) priv;
        unsigned char* BUFFER[sizeof(uint32_t)];
	PackUInt32(BUFFER, DESIRED_ATTITUDE);

        // Unpack attitude_t structure
        attitude_t receivedAttitude;
        UnpackAttitudeData((unsigned char*) data, &receivedAttitude);

        // Verify  and Save Configuration for Control
        if(setAttitude(&receivedAttitude))
        {		         
        	// Responed with command_ack and set_config
        	udp_send(self->sock, src, COMMAND_ACK, BUFFER, sizeof(uint32_t));
     	}

        fprintf(stderr,"receivedAttitude \nAttitude = %f %f %f", receivedAttitude.phi, receivedAttitude.theta, receivedAttitude.psi);
	return 1;
}

static int save_config(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	Server *self = (Server*) priv;
        unsigned char* BUFFER[sizeof(uint32_t)];
	PackUInt32(BUFFER, SAVE_CONFIG);

        if(saveConfig())
        {		         
        	// Responed with command_ack and set_config
        	udp_send(self->sock, src, COMMAND_ACK, BUFFER, sizeof(uint32_t));
     	}
        fprintf(stderr,"save_config requested"); 

	return 1;
}

static int get_config(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	return 1;
}

static int vicon_state(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	Server *self = (Server*) priv;

        // Unpack vicon_state_t structure
        pthread_mutex_lock(&viconMutex);
        UnpackViconState((unsigned char*) data, &viconState);
        pthread_mutex_unlock(&viconMutex);

       fprintf(stderr,"%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf",viconState.x,viconState.y,viconState.z,viconState.vx,viconState.vy,viconState.vz,viconState.phi,viconState.theta,viconState.psi);

	return 1;
}


void serve(Server *server, int port)
{
	//port = 0;
	if( server->sock >= 0 )
		close( server->sock );

	// Create the socket
	server->sock = udp_serve( port );

	if(server->sock < 0 ){
		perror( "socket" );
		return;
	}

	int yes = 1;

	if( setsockopt(server->sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0 ){
        	perror("setsockopt");
		return;
	}

	Host_t			self;


	udp_self(server->sock, &self );
	fprintf(stderr, "UDP server created: socket on fd = %d on port %d \n", server->sock, ntohs( self.sin_port ));




	// Zero our handler table and install the defaults
	int i;
	for( i=0 ; i < COMMAND_MAX ; i++ )
        {
		server_handle(server, i, 0, 0 );
        }

	server_handle(server, COMMAND_NOP, command_nop, 0 );
	server_handle(server, COMMAND_ACK, command_nop, 0 );
	server_handle(server, COMMAND_OPEN, command_open, (void*) server );
	server_handle(server, COMMAND_CLOSE, command_close, (void*) server );
        
        // Additional Messages
        server_handle(server, SET_CONFIG, set_config, (void*) server );
        server_handle(server, GAINS, set_gains, (void*) server );
        server_handle(server, PARAMETERS, set_parameters, (void*) server );
        server_handle(server, DESIRED_POSITION, desired_position, (void*) server );
        server_handle(server, DESIRED_ATTITUDE, desired_attitude, (void*) server );
        server_handle(server, SAVE_CONFIG, save_config, (void*) server );
        server_handle(server, GET_CONFIG, get_config, (void*) server );
        server_handle(server, VICON_STATE, vicon_state, (void*) server );

}

void server_init(Server *server, int port)	
{
	server->sock = -1;
	server->last_packet.tv_usec	= 0;
	server->last_packet.tv_sec	= 0;

	serve( server, port );
}

void server_connect(Server *server,const char *hostname,int port)
{
	if( host_lookup(&(server->server), hostname, port ) < 0 )
	{
		perror( hostname );
		exit(-1);
		return;
	}

	/* Try to connect */
	udp_send(server->sock, &(server->server),COMMAND_OPEN, 0, 0);


	server_add_client(server,&(server->server));

}

void server_send_packet(Server *server, int type, const void *buf,size_t len)
{
	struct timeval		now;
	gettimeofday( &now, 0 );
	now.tv_sec		= htonl( now.tv_sec );
	now.tv_usec		= htonl( now.tv_usec );
	type			= htonl( type );
	


	if(client_added){
	  int i;
	  for(i = 0; i < SERVER_MAX_CLIENTS; i++){
	    const Host_t *client1 = &client[i];
	    //int msg_len = udp_send_raw(server->sock, (client1), type, &now, buf, len);
	  
		if (client1->sin_port == 0)
		{
			// We have reached the end of actual clients stored in the array
			break;
		}
		  
	    int msg_len = udp_send_raw(server->sock, (client1), type, &now, buf, len);


		}
	}


}
void server_send_parameter(Server *server, int type, double value)
{
	
	const Host_t *client1 = &client[0];
	
	udp_send(server->sock,client1,type, &value, sizeof(value));


}


int server_poll(Server *server, int usec)
{

	return udp_poll(server->sock, 1 );

}

void server_send_command(Server *server, int type)
{



	udp_send(server->sock,&(server->server),type,0,0);

}


int server_get_packet(Server *server)
{
	int			len;
	Host_t			src;
	char			buf[ 1024 ];
	char *			data;
	struct timeval *	when;
	uint32_t		type;


	len = udp_read(server->sock, &src, buf, sizeof(buf) );
	len -= sizeof( struct timeval ) + sizeof( uint32_t );
	data = udp_parse( buf, &when, &type );

	if( type > COMMAND_MAX )
	{
		fprintf(stderr, "Invalid packet of type %d\n", type);
		return -1;
	}

	if( when->tv_sec == server->last_packet.tv_sec
	&&  when->tv_usec == server->last_packet.tv_usec)
	{
		/* Duplicate packet! */
		printf("Duplicate Packet received! (or at least one with the exact same timestamp as the last one)\n");
		return type;
	}

	server->last_packet = *when;


	if( !server->handlers[type].func )
	{
		fprintf(stderr, "Unhandled packet of type %d\n", type);
	}
	else
	{
		// Run the message handler
		if (!server->handlers[type].func(server->handlers[type].data,&src,type,when,data,len))
		{
			fprintf(stderr, "Message-handler for message-type: %d, dismissed the message\n", type);
		}
	}

	return type;
}

void server_update(int fd,void *voidp)
{
	Server *server = (Server*) voidp;
	server_get_packet(server);
}


/* this function handles helicopter state messages heli_state_t defined in state.h */

extern int server_process_state(void *priv,const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	static struct timeval 	last_packet;
	
	state_t *state = (state_t *)priv;
	
	if( len != sizeof( state_t )){
	  fprintf(stderr, "Invalid AHRS packet\n");
	  return;
	}
	
	if( timercmp( &last_packet, when, > ) ){
	  fprintf(stderr, "old AHRS packet\n");
	  return;
	}
	
	last_packet	= *when;
	
	ntoh_state( (const state_t*) data, state );
	
	
	
}

/* this function handles camera messages cam_state_t defined in state.h */

extern int server_cam_state(void *priv,const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	static struct timeval 	last_packet;
	
	msg_camera_t *state = (msg_camera_t *)priv;
	
	if( len != sizeof( msg_camera_t )){
		fprintf(stderr, "Invalid AHRS packet\n");
		return;
	}
	
	if( timercmp( &last_packet, when, > ) ){
		fprintf(stderr, "old AHRS packet\n");
		return;
	}
	
	last_packet	= *when;
	
	ntoh_cam( (const msg_camera_t*) data, state );
	
	
}


/* this function handles flyto messages heli_flyto_state_t defined in state.h */

extern int server_flyto_state(void *priv,const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	static struct timeval 	last_packet;
	
	msg_flyto_t *state = (msg_flyto_t *)priv;
	
	if( len != sizeof( msg_flyto_t )){
		fprintf(stderr, "Invalid AHRS packet\n");
		return;
	}
	
	if( timercmp( &last_packet, when, > ) ){
		fprintf(stderr, "old AHRS packet\n");
		return;
	}
	
	last_packet	= *when;
	
	ntoh_heli_flyto( (const msg_flyto_t*) data, state );
	
}

int server_default_message_handler_imode_state(void *priv, const Host_t *src, int type, const struct timeval *when, const void *data, size_t len)
{
	static struct timeval lastMessageTimestamp;
	
	// Type cast the pointer to the data-store (priv) to a iphone_primary_input_sample_t pointer
	iphone_primary_input_sample_t *smplDestination = (iphone_primary_input_sample_t *)priv;
	
	// Check that the message is more recent than the last one recieved, of this type	
	if (timercmp(&lastMessageTimestamp, when, >))
	{
		fprintf(stderr, "Incoming IMODE_STATE message has expired - a newer one has already been processed\n");
		return 0;
	}
	lastMessageTimestamp = *when;
	
	// Check that the message payload is valid
	if (!ValidatePackedIphonePrimaryInputSample(data, len))
	{
		fprintf(stderr, "Incoming IMODE_STATE message payload failed validation - probably due to length: %d\n", (int)len);
		return 0;
	}

	// Unserialise the payload
	UnpackIphonePrimaryInputSample(data, smplDestination);
	
	return 1;
}


