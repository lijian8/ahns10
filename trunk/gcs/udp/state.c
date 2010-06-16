/* $Author$
 * $Name:  $
 * $Id$
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
/******************* START - iphone related content - START *******************/
/******************************************************************************/

// This is used to convert a variable of type, msg_iphone_primary_input_sample_t, from network byte order into host byte order
// I don't know why it's flagged as 'extern'
// This function should be deprecated - UnpackIphonePrimaryInputSample() and PackIphonePrimaryInputSample() should be used instead
extern void ntoh_imode(const iphone_primary_input_sample_t *net_state, iphone_primary_input_sample_t *state)
{
 	*state = *net_state;
}

void PrintIphonePrimarySample(iphone_primary_input_sample_t sample, int indentationDegree, FILE *outputStream)
{
	char indentationChars[100];
	memset(indentationChars, '\t', indentationDegree);
	indentationChars[indentationDegree] = '\0';

	fprintf(outputStream, "%siphone_primary_input_sample_t :: timestamp: %lf, wasPaused: %s", indentationChars, sample.timestamp, (sample.wasControllerPaused ? "YES" : " NO"));
	if (sample.lateralIntensity.isValid)
	{
		fprintf(outputStream, ", lat: %9.4f", sample.lateralIntensity.value);
	}
	else
	{
		fprintf(outputStream, ", lat: <invalid>");
	}
	
	if (sample.longitudinalIntensity.isValid)
	{
		fprintf(outputStream, ", long: %9.4f", sample.longitudinalIntensity.value);
	}
	else
	{
		fprintf(outputStream, ", long: <invalid>");
	}
	
	if (sample.yawIntensity.isValid)
	{
		fprintf(outputStream, ", yaw: %9.4f", sample.yawIntensity.value);
	}
	else
	{
		fprintf(outputStream, ", yaw: <invalid>");
	}
	
	if (sample.collectiveLiftIntensity.isValid)
	{
		fprintf(outputStream, ", lift: %9.4f", sample.collectiveLiftIntensity.value);
	}
	else
	{
		fprintf(outputStream, ", lift: <invalid>");
	}
	
	fprintf(outputStream, "\n");
	
}

int ValidatePackedIphonePrimaryInputSample(const void *packedData, int packedDataLength)
{
	// All we can currently do is check that the length is correct, based on the 
	// current method of serialising iphone_primary_input_sample_t objects
	
	// Note that the packed version can and usually is smaller than 
	// sizeof(struct) on account of struct padding
	
	return (packedDataLength == IPHONE_PRIMARY_INPUT_SAMPLE_T_SERIALISED_SIZE) ? 1 : 0;
}

int UnpackIphonePrimaryInputSample(const unsigned char *packedData, iphone_primary_input_sample_t *smplDestination)
{
	int dataOffset = 0;
	
	// Lateral Intensity
	dataOffset += UnpackFloat32(&packedData[dataOffset], &smplDestination->lateralIntensity.value);
	dataOffset += UnpackUInt8(&packedData[dataOffset], &smplDestination->lateralIntensity.isValid);

	// Longitudinal Intensity
	dataOffset += UnpackFloat32(&packedData[dataOffset], &smplDestination->longitudinalIntensity.value);
	dataOffset += UnpackUInt8(&packedData[dataOffset], &smplDestination->longitudinalIntensity.isValid);
	
	// Yaw Intensity
	dataOffset += UnpackFloat32(&packedData[dataOffset], &smplDestination->yawIntensity.value);
	dataOffset += UnpackUInt8(&packedData[dataOffset], &smplDestination->yawIntensity.isValid);

	// Lift Intensity
	dataOffset += UnpackFloat32(&packedData[dataOffset], &smplDestination->collectiveLiftIntensity.value);
	dataOffset += UnpackUInt8(&packedData[dataOffset], &smplDestination->collectiveLiftIntensity.isValid);
	
	// The others
	dataOffset += UnpackFloat64(&packedData[dataOffset], &smplDestination->timestamp);
	dataOffset += UnpackUInt8(&packedData[dataOffset], &smplDestination->wasControllerPaused);
	
	return dataOffset;
}

int PackIphonePrimaryInputSample(unsigned char *packingBuffer, iphone_primary_input_sample_t smplSource)
{
	/*
	 * Note:	This implementation makes use of the functions defined in primitive_serialisation.h/.c.
	 *			These functions handle the issue of converting from host byte-order to network byte-order
	 *			by always explicitly converting the logical bitwise representation of the primitive
	 *			data-type into a string representation in network byte-order (and back again to unpack).
	 *			Unless I'm horribly deluded as to how this can be done.
	 *
	 *			There is probably a performance hit in utilising the serialisation functions of
	 *			primitive_serialisation, as oppossed to alternate methods that use hton functions.
	 *
	 *			All of the data-types that can be serialised in primitive_serialisation.h/.c are ones
	 *			that have explicit sizes defined (uint8_t, uint16_t, int32_t, float32, etc).
	 *			This should make them portable when serialising/unserialising data within programs
	 *			that work in different memory environments, ie 32 bit vs 64 bit.  However none of this
	 *			has been tested extensively.
	 */
	int bufOffset = 0;

	// Lateral Intensity
	bufOffset += PackFloat32(&packingBuffer[bufOffset], smplSource.lateralIntensity.value);
	bufOffset += PackUInt8(&packingBuffer[bufOffset], smplSource.lateralIntensity.isValid);

	// Longitudinal Intensity
	bufOffset += PackFloat32(&packingBuffer[bufOffset], smplSource.longitudinalIntensity.value);
	bufOffset += PackUInt8(&packingBuffer[bufOffset], smplSource.longitudinalIntensity.isValid);
	
	// Yaw Intensity
	bufOffset += PackFloat32(&packingBuffer[bufOffset], smplSource.yawIntensity.value);
	bufOffset += PackUInt8(&packingBuffer[bufOffset], smplSource.yawIntensity.isValid);
	
	// Lift Intensity
	bufOffset += PackFloat32(&packingBuffer[bufOffset], smplSource.collectiveLiftIntensity.value);
	bufOffset += PackUInt8(&packingBuffer[bufOffset], smplSource.collectiveLiftIntensity.isValid);

	// The others
	bufOffset += PackFloat64(&packingBuffer[bufOffset], smplSource.timestamp);
	bufOffset += PackUInt8(&packingBuffer[bufOffset], smplSource.wasControllerPaused);
	
	return bufOffset;
}




/******************************************************************************/
/********************* END - iphone related content - END *********************/
/******************************************************************************/

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
