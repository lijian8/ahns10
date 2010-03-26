/**
 * @file
 * @author Brendan Chen
 *
 * Last Modified by: $Author: tb.chen.87 $ 
 *
 * $LastChangedDate: 2009-10-06 13:55:05 +1000 (Tue, 06 Oct 2009) $
 * $Rev: 749 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 *
 * This file defines details for the HMI packet
 *
 */

#ifndef AHNS_HMI_PACKET
#define AHNS_HMI_PACKET

#define HMI_BUFFSIZE 19			// +1 For end of line character
#define HMI_PACKSIZE (HMI_BUFFSIZE-4) 	// Always minus 4 SYNC bytes: Used for the receiving end

/* ******************************************* */
/* ********** DEFINE MESSAGE MACROS ********** */
/* ******************************************* */
#define HMI_SYNC_1 0xFC
#define HMI_SYNC_2 0xFD
#define HMI_SYNC_3 0xFE
#define HMI_SYNC_4 0xFF

namespace hmi_packet{

/**
 * The enumeration for the HMI receive state machine.
 */
enum hmi_state
{
	S1, S2, S3, S4, S5, S6, S7, S8, S9
};

/* ******************************************* */
/* ********** DEFINE COMMAND MACROS ********** */
/* ******************************************* */
#define AUTO 0x01 // Autonomous control flag
#define STAB 0x02 // Stability Augmentation control flag

/*#define UDP_0 11 // UDP Established

#define TLM_0 1*/
}

#endif
