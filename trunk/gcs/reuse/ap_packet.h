/**
 * @file
 * @author Brendan Chen
 *
 * Last Modified by: $Author: wainwright.alex $ 
 *
 * $LastChangedDate: 2009-10-06 13:52:15 +1000 (Tue, 06 Oct 2009) $
 * $Rev: 751 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 *
 * This file defines details for the HMI packet
 *
 */

#ifndef AHNS_AP_PACKET
#define AHNS_AP_PACKET

#define AP_BUFFSIZE 34
#define AP_PACKSIZE (AP_BUFFSIZE-4) // Always minus 4 SYNC bytes: Used for the receiving end

/* ******************************************* */
/* ********** DEFINE MESSAGE MACROS ********** */
/* ******************************************* */
#define AP_SYNC_1 0xFC
#define AP_SYNC_2 0xFD
#define AP_SYNC_3 0xFE
#define AP_SYNC_4 0xFF

#define ALL_BAD		0xBB
#define GP_GOOD		0x0A
#define GP_BAD		0x0B
#define TEL_GOOD	0xA0
#define TEL_BAD		0xB0

#define AP_STATE_1 1
#define AP_STATE_2 2
#define AP_STATE_3 3
#define AP_STATE_4 4
#define AP_STATE_5 5
#define AP_STATE_6 6
#define AP_STATE_7 7
#define AP_STATE_8 8
#define AP_STATE_9 9

#endif

