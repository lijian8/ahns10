/* $Author: tlmolloy $
 * $Name:  $
 * $Id: query_state1.c 164 2010-06-10 13:59:05Z tlmolloy $
 * $Source: /home/luis/cvsroot/net-api/query_state1.c,v $
 * $Log: query_state1.c,v $
 * Revision 1.1.1.1  2008-05-05 07:07:57  luis
 * initial import
 *
 * Revision 1.1.1.1  2008-05-05 06:49:35  luis
 * initial import
 *
 * Revision 1.2  2006/01/18 15:47:45  cvs
 * *** empty log message ***
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

/******************************************
 *             CLIENT SIDE OF THE EXAMPLE
 *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "state.h"
#include "server.h"
#include "commands.h"
#include "udp.h"


int verbose = 0;
int server_port	= 2002;

char *server_host;
static Server server;
unsigned char   init =1;
struct timeval gnd_time, local_time;
char *file;
char text[128];

int main(int argc, char *argv[])
{
  const char *program	= argv[0];

  int c, errflag = 0;


    server_host = (char *) malloc(100*sizeof(char));
    file = (char *) malloc(100*sizeof(char));

    if(server_host == NULL){
        fprintf(stderr, "cannot allocate mem\n");
        exit(-1);
    }

 
    server_host = "127.0.0.1";
    
    while((c = getopt(argc, argv, "s:p:v:")) != -1){
        switch(c){
            case 'p':
                server_port = atoi(optarg);
                fprintf(stderr, "port %d\n", server_port);
                break;
            case 's':
                server_host = optarg;
                fprintf(stderr, "server %s\n", server_host);
                break;
            case 'v':
                verbose = atoi(optarg);
                fprintf(stderr, "verbose %d\n", verbose);
                break;
            case '?':
                ++errflag;
                break;
        }
    }


    state_t state;

    
    
    int count=0;
    int type;
    
    server_init(&server, 0); // creates socket and binds to port 0
    server_connect(&server, server_host, server_port); 
    
    server_handle(&server, HELI_STATE, server_process_state, (void*) &state);
      
    memset((void *)(&state), 0, sizeof(state_t));

   
  
    while( 1 ){
	
      gettimeofday(&gnd_time,0);
      int type=0;
      if(server_poll(&server, 1))
	type = server_get_packet(&server);
      
      
      if( type < 0 ){
	fprintf(stderr,"Read error from server");
	return -1;
	
      }
	
	
     count++;
	
	//Display packages coming into the socket
	printf( "\n\nReceiving\n");
	printf("heli state : %f %f.%f\n", state.phi, state.theta, state.psi );
	
	usleep(100000);
	
	        

        //send data to the socket
        if(count%1 == 0)
            init = 1;

		/*
        if(init){

	  memset((void *)(&state_out), 0, sizeof(MSG_SERVER));
	  
          state_out.Id = count;
	  state_out.com1 = 2.0*(double)rand()/(double)RAND_MAX;
	  state_out.com2 =3.14*(double)rand()/(double)RAND_MAX;
	  state_out.com3=1.5*(double)rand()/(double)RAND_MAX;
	  state_out.com4 = 10.0*(double)rand()/(double)RAND_MAX;
	  state_out.com5 = 0.0;
	  state_out.time.tv_sec=gnd_time.tv_sec;
	  state_out.time.tv_usec=gnd_time.tv_usec;
	  
            server_send_packet(&server, COMM, &state_out, sizeof(MSG_CLIENT));
            
 	    printf( "\n\nSending COMM \n"); 
	    printf(": %d %d %d %d %d %d %d.%d\n",
		   state_out.Id ,
		   state_out.com1 ,
		   state_out.com2,
		   state_out.com3,
		   state_out.com4,
		   state_out.com5,
		   state_out.time.tv_sec,
		   state_out.time.tv_usec
 	    );


            init = 0;
        }
        
        
 	fprintf(fd, "%d %d.%d : %d.%d %d %d %d %d %d %d %d.%d\n",
		state_in.nPacket,
		state_in.time.tv_sec,
		state_in.time.tv_usec,
		gnd_time.tv_sec,
		gnd_time.tv_usec,
		state_out.Id ,
		state_out.com1 ,
		state_out.com2,
		state_out.com3,
		state_out.com4,
		state_out.com5,   
		state_out.time.tv_sec,
		state_out.time.tv_usec

	       );
		 */
		
	}

    //fclose(file);
    return 0;
}

