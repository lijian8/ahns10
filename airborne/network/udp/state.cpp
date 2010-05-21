/* $Author: tlmolloy $
 * $Name:  $
 * $Id: state.c 155 2010-05-21 10:15:47Z tlmolloy $
 * $Source: /home/luis/cvsroot/net-api/state.c,v $
 * $Log: state.c,v $
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
 * Revision 1.1.1.1  2005/10/15 15:14:06  cvs
 * initial project comments
 *
 * Revision 1.1.1.1  2004/03/03 11:03:06  srik
 * Initial Release 0.1
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>

#include "state.h"
#include "primitive_serialisation.h"



extern void ntoh_state(const state_t *net_state, state_t *state)

{
 	*state = *net_state; 

}

extern void ntoh_cam(const msg_camera_t *net_state, msg_camera_t *state)

{
 	*state = *net_state; 
	
}

extern void ntoh_heli_flyto(const msg_flyto_t *net_state, msg_flyto_t *state)

{
 	*state = *net_state; 
	
}

/******************************************************************************/
/********************* START - GCS related content - START ********************/
/******************************************************************************/

int PackFCState(unsigned char *packedData, fc_state_t *srcState)
{
    int bufOffset = 0;

    // Commanded Engine
    bufOffset += PackUInt16(&packedData[bufOffset], srcState->commandedEngine1);
    bufOffset += PackUInt16(&packedData[bufOffset], srcState->commandedEngine2);
    bufOffset += PackUInt16(&packedData[bufOffset], srcState->commandedEngine3);
    bufOffset += PackUInt16(&packedData[bufOffset], srcState->commandedEngine4);

    // RC Link
    bufOffset += PackUInt8(&packedData[bufOffset], srcState->rclinkActive);

    // FC Uptime and Usage
    bufOffset += PackUInt64(&packedData[bufOffset], srcState->fcUptime);
    bufOffset += PackUInt8(&packedData[bufOffset], srcState->fcCPUusage);

    return bufOffset;
}

int UnpackFCState(unsigned char *packedData, fc_state_t *stateDestination)
{
    int dataOffset = 0;

    // Commanded Engine
    dataOffset += UnpackUInt16(&packedData[dataOffset], &stateDestination->commandedEngine1);
    dataOffset += UnpackUInt16(&packedData[dataOffset], &stateDestination->commandedEngine2);
    dataOffset += UnpackUInt16(&packedData[dataOffset], &stateDestination->commandedEngine3);
    dataOffset += UnpackUInt16(&packedData[dataOffset], &stateDestination->commandedEngine4);

    // RC Link
    dataOffset += UnpackUInt8(&packedData[dataOffset], &stateDestination->rclinkActive);

    // FC Uptime and Usage
    dataOffset += UnpackUInt64(&packedData[dataOffset], &stateDestination->fcUptime);
    dataOffset += UnpackUInt8(&packedData[dataOffset], &stateDestination->fcCPUusage);

    return dataOffset;
}

int PackAPState(unsigned char *packedData, ap_state_t *srcState)
{
    int bufOffset = 0;

    // Reference Commands
    bufOffset += PackFloat64(&packedData[bufOffset], srcState->referencePhi);
    bufOffset += PackFloat64(&packedData[bufOffset], srcState->referenceTheta);
    bufOffset += PackFloat64(&packedData[bufOffset], srcState->referencePsi);
    bufOffset += PackFloat64(&packedData[bufOffset], srcState->referenceX);
    bufOffset += PackFloat64(&packedData[bufOffset], srcState->referenceY);
    bufOffset += PackFloat64(&packedData[bufOffset], srcState->referenceZ);

    // Active Loops
    bufOffset += PackUInt8(&packedData[bufOffset], srcState->phiActive);
    bufOffset += PackUInt8(&packedData[bufOffset], srcState->thetaActive);
    bufOffset += PackUInt8(&packedData[bufOffset], srcState->psiActive);
    bufOffset += PackUInt8(&packedData[bufOffset], srcState->xActive);
    bufOffset += PackUInt8(&packedData[bufOffset], srcState->yActive);
    bufOffset += PackUInt8(&packedData[bufOffset], srcState->zActive);

    return bufOffset;
}

int UnpackAPState(const unsigned char *packedData, ap_state_t *stateDestination)
{
    int dataOffset = 0;

    //Reference Commands
    dataOffset += UnpackFloat64(&packedData[dataOffset], &stateDestination->referencePhi);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &stateDestination->referenceTheta);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &stateDestination->referencePsi);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &stateDestination->referenceX);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &stateDestination->referenceY);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &stateDestination->referenceZ);

    // Active Loops
    dataOffset += UnpackUInt8(&packedData[dataOffset], &stateDestination->phiActive);
    dataOffset += UnpackUInt8(&packedData[dataOffset], &stateDestination->thetaActive);
    dataOffset += UnpackUInt8(&packedData[dataOffset], &stateDestination->psiActive);
    dataOffset += UnpackUInt8(&packedData[dataOffset], &stateDestination->xActive);
    dataOffset += UnpackUInt8(&packedData[dataOffset], &stateDestination->yActive);
    dataOffset += UnpackUInt8(&packedData[dataOffset], &stateDestination->zActive);

    return dataOffset;
}

int PackGains(unsigned char *packedData, gains_t *srcGains)
{
    int bufOffset = 0;

    // Gains to network order
    bufOffset += PackFloat64(&packedData[bufOffset], srcGains->kp);
    bufOffset += PackFloat64(&packedData[bufOffset], srcGains->ki);
    bufOffset += PackFloat64(&packedData[bufOffset], srcGains->kd);

    return bufOffset;
}

int UnpackGains(const unsigned char *packedData, gains_t *gainsDestination)
{
    int dataOffset = 0;

    // Gains from network order
    dataOffset += UnpackFloat64(&packedData[dataOffset], &gainsDestination->kp);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &gainsDestination->ki);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &gainsDestination->kd);

    return dataOffset;
}


int PackParametersState(unsigned char *packedData, loop_parameters_t *srcParam)
{
    int bufOffset = 0;

    // Gains to network order
    bufOffset += PackFloat64(&packedData[bufOffset], srcParam->maximumOutput);
    bufOffset += PackFloat64(&packedData[bufOffset], srcParam->minimumOutput);
    bufOffset += PackFloat64(&packedData[bufOffset], srcParam->neutralOutput);

    return bufOffset;
}

int UnpackParametersState(const unsigned char *packedData, loop_parameters_t *paramDestination)
{
    int dataOffset = 0;

    // Gains to network order
    dataOffset += UnpackFloat64(&packedData[dataOffset], &paramDestination->maximumOutput);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &paramDestination->minimumOutput);
    dataOffset += UnpackFloat64(&packedData[dataOffset], &paramDestination->neutralOutput);

    return dataOffset;
}

/******************************************************************************/
/*********************** END - GCS related content - END **********************/
/******************************************************************************/
