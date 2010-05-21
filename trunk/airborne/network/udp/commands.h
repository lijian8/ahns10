/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/commands.h,v $
 * $Log: commands.h,v $
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
#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#define C_DEG2RAD        0.017453292519943295
#define C_DEG2RAD_F      0.017453292519943295f
#define C_RAD2DEG       57.295779513082323
#define C_RAD2DEG_F     57.295779513082323f

enum {
    COMMAND_NOP		    = 0,
    COMMAND_OPEN 	    = 1,
    COMMAND_ACK		    = 2,
    COMMAND_CLOSE	    = 3,
	
    SIM_QUIT		    = 4,
    SIM_RESET		    = 5,
    SAVE_CONFIG		    = 6,
    GET_CONFIG		    = 7,
    SET_CONFIG		    = 8,
    /* currently implemented */
    HELI_STATE		    = 10,
    CAM_STATE           = 11,
    GPS_STATE           = 12,
    IMODE_STATE         = 13, /**< This command relates to the msg_iphone_primary_input_sample_t data type */
    AUTOPILOT_STATE     = 14,
    FC_STATE            = 15,

    FLYTO               = 16,
	 /* currently implemented */
	
    TANGENT_PLANE       = 17,
    COMMAND_MODE        = 18,
    PLATFORM            = 19,
    DESIRED_POSITION    = 20,
    TELEOP_CMD          = 21,
    FAILSAFE            = 22,
    VELSCALE            = 23,
    MAGCAL              = 24,
    FADEC               = 25,
    
    DESIRED_ATTITUDE    = 30,

    ATTITUDE_GAIN_ROLL	= 50,
    ATTITUDE_GAIN_PITCH	= 51,
    ATTITUDE_GAIN_YAW	= 52,
    VELOCITY_GAIN_X     = 53,
    VELOCITY_GAIN_Y     = 54,
    VELOCITY_GAIN_Z     = 55,
    GUIDANCE_GAIN_X	= 56,
    GUIDANCE_GAIN_Y	= 57,
    GUIDANCE_GAIN_Z	= 58,

    ATTITUDE_PARAMETERS_ROLL  = 60,
    ATTITUDE_PARAMETERS_PITCH = 61,
    ATTITUDE_PARAMETERS_YAW   = 62,

    GUIDANCE_PARAMETERS_X     = 66,
    GUIDANCE_PARAMETERS_Y     = 67,
    GUIDANCE_PARAMETERS_Z     = 68,

    SERVO_ROLL		= 80,
    SERVO_PITCH		= 81,
    SERVO_YAW		= 82,
    SERVO_COLL		= 83,
    SERVO_ANTENNA	= 84,
    SIM_DISPLACE        = 90,
    VISION_STATE        = 200,
    VISION_COMMANDS     = 201,

    NOTE                = 255,

    COMMAND_MAX         = 256
};

#endif
