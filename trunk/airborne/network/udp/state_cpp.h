/* $Author$
 * $Rev$
 * $LastChangedDate$
 * $Id$
 *
 * 
 */

/**
 *  \file state.h
 *  \brief The main procedure. It can do the following:
 *  - do nothing
 *  - sleep
 *
 * @author Luis Mejias
 */



#ifndef _STATE_H_
#define _STATE_H_

#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <math.h>
/**
 *   \brief state data of the helicopter
 */

typedef struct {

	/** @name Euler angles relative to the ground */
	double		phi;     /**< roll angle */
	double		theta;   /**< pitch  angle */  
	double		psi;     /**< yaw angle */

	/** @name Body frame rotational rates */
	double		p; /**< roll rate */
	double		q; /**< pitch rate */
	double		r; /**< yaw rate */

	/** @name Position relative to the ground */
	double		x; /**< the x coordinate in the local frame */
	double		y; /**< the y coordinate in the local frame */
	double		z; /**< the z coordinate in the local frame */

	/** @name Velocity over the ground */
	double		vx; /**< longitudinal velocity */
	double		vy; /**< lateral velocity */
	double		vz; /**< vertical velocity */

	/** @name Body frame linear accelerations */
	double		ax; /**< longitudinal acceleration */
	double		ay; /**< lateral acceleration */
	double		az; /**< vertical acceleration */

	/** @name Miscellaneous */
	double		trace;   
	double		voltage; 


} state_t;

/**
 *    \brief flyto command. used to send the helicopter to a specific location
 */
    
typedef struct {
    int16_t     var7; /**< to be defined */
    int16_t     var6; /**< to be defined */
    uint16_t    var5; /**< to be defined */
    double      var4; /**< to be defined */
    double      var3; /**< to be defined */
    double      var2; /**< to be defined */
    double      var1; /**< to be defined */
} msg_flyto_t;

/**
 *   \brief camera data structure
 */

typedef struct {
      int16_t     var7; /**< to be defined */
    int16_t     var6; /**< to be defined */
    uint16_t    var5; /**< to be defined */
    double      var4; /**< to be defined */
    double      var3; /**< to be defined */
    double      var2; /**< to be defined */
    double      var1; /**< to be defined */
} msg_camera_t;

/****************** START - GCS related definitons - START *****************/


/**
 *   \brief Flight Computer state structure
 *
 *   To be sent periodically to the GCS
 */
typedef struct {
    /** @name Commanded Engine Inputs */
    uint16_t            commandedEngine1;  /**< Engine 1 PWM*/
    uint16_t		commandedEngine2;   /**< Engine 2 PWM*/
    uint16_t		commandedEngine3;   /**< Engine 3 PWM*/
    uint16_t            commandedEngine4;   /**< Engine 4 PWM*/

    /** @name Radio Control Link Status */
    uint8_t                rclinkActive;       /**< RC connected or not */

    /** @name Flight Computer Status*/
    uint64_t            fcUptime;   /**< FC Uptime in seconds*/
    uint8_t             fcCPUusage; /**< percentage FC usage*/
} fc_state_t;

/**
 *   \brief Packs Flight Computer state structure
 *
 *   Packs the struct fc_state_t to network byte order and places it in the buffer .
 *   Uses the primitive_serialisation.h/c,
 *   Thanks to Joel.
 *
 *   @param   packingBuffer  unsigned char* buffer to which the serialised representation of the object will be stored
 *   @param   fc_state_t  srcState  state to be serialised
 *   @return  int  number of bytes written from packedData (the length of the serialised object)
 */
int PackFCState(unsigned char *packedData, fc_state_t *srcState);

/**
 *   \brief Unpacks Flight Computer state structure
 *
 *   Unpacks the serialisaed buffer to the struct fc_state_t.
 *   Uses the primitive_serialisation.h/c,
 *   Thanks to Joel.
 *
 *   @param   packingBuffer  unsigned char* buffer to which the serialised representation of the object will be stored
 *   @param   fc_state_t  stateDestination  state to be unserialised
 *   @return  int  number of bytes written from packedData (the length of the serialised object)
 */
int UnpackFCState(unsigned char *packedData, fc_state_t *stateDestination);

/**
 *   \brief State data of the autopilot
 *
 *   To be sent periodically to the GCS to monitor autopilot operation
 */
typedef struct {
    /** @name Attitude Control Loop References */
    double		referencePhi;   /**< reference roll angle */
    double		referenceTheta; /**< reference pitch angle */
    double		referencePsi;   /**< reference yaw angle */

    /** @name Guidance Control Loop References */
    double		referenceX; /**< reference x position */
    double		referenceY; /**< reference position */
    double		referenceZ; /**< reference yaw position */

    /** @name Active Attitude Control Loops */
    uint8_t             phiActive;     /**< roll loop */
    uint8_t	        thetaActive;   /**< pitch  loop */
    uint8_t		psiActive;     /**< yaw loop */

    /** @name Active Guidance Loops */
    uint8_t             xActive;     /**< x loop */
    uint8_t	        yActive;     /**< y  loop */
    uint8_t		zActive;     /**< z loop */
} ap_state_t;

/**
 *   \brief Packs Autopilot state structure
 *
 *   Packs the struct ap_state_t to network byte order and places it in the buffer .
 *   Uses the primitive_serialisation.h/c,
 *   Thanks to Joel.
 *
 *   @param   packingBuffer  unsigned char* buffer to which the serialised representation of the object will be stored
 *   @param   ap_state_t  srcState  state to be serialised
 *   @return  int  number of bytes written from packedData (the length of the serialised object)
 */
int PackAPState(unsigned char *packedData, ap_state_t *srcState);

/**
 *   \brief Unpacks Autopilot state structure
 *
 *   Unpacks the serialisaed buffer to the struct fc_state_t.
 *   Uses the primitive_serialisation.h/c,
 *   Thanks to Joel.
 *
 *   @param   packingBuffer  unsigned char* buffer to which the serialised representation of the object will be stored
 *   @param   fc_state_t  stateDestination  state to be unserialised
 *   @return  int  number of bytes written from packedData (the length of the serialised object)
 */
int UnpackAPState(const unsigned char *packedData, ap_state_t *stateDestination);

/**
 *   \brief Gain Structure for Autopilot
 *
 *   To be sent and received on demand by the GCS to configure the autopilot loops
 */

typedef struct {
    /** @name PID Control Loop Gains*/
    double		kp;   /**< proportional gain */
    double		ki;   /**< integral gain */
    double		kd;   /**< rate gain */
} gains_t;

/**
 *   \brief Packs Gains structure
 *
 *   Packs the struct gains_t to network byte order and places it in the buffer .
 *   Uses the primitive_serialisation.h/c,
 *   Thanks to Joel.
 *
 *   @param   packingBuffer  unsigned char* buffer to which the serialised representation of the object will be stored
 *   @param   gains_t  srcGains Gains to be serialised
 *   @return  int  number of bytes written from packedData (the length of the serialised object)
 */
int PackGains(unsigned char *packedData, gains_t *srcGains);

/**
 *   \brief Unpacks Gains structure
 *
 *   Unpacks the serialisaed buffer to the struct gains_t.
 *   Uses the primitive_serialisation.h/c,
 *   Thanks to Joel.
 *
 *   @param   packingBuffer  unsigned char* buffer to which the serialised representation of the object will be stored
 *   @param   gains_t  gainsDestination  Gains to be unserialised
 *   @return  int  number of bytes written from packedData (the length of the serialised object)
 */
int UnpackGains(const unsigned char *packedData, gains_t *gainsDestination);

/**
 *   \brief Control Loop Parameters
 *
 *   To be sent and received on demand by the GCS to configure the control inputs given
 */

typedef struct {
    /** @name Maximum, Minimum and Netural Outputs*/
    double		maximumOutput;   /**< maximum loop commanded output*/
    double              minimumOutput;   /**< minimum loop commanded output */
    double              neutralOutput;   /**< neutral commanded output*/
} loop_parameters_t;

/**
 *   \brief Packs Loop Parameter structure
 *
 *   Packs the struct gains_t to network byte order and places it in the buffer .
 *   Uses the primitive_serialisation.h/c,
 *   Thanks to Joel.
 *
 *   @param   packingBuffer  unsigned char* buffer to which the serialised representation of the object will be stored
 *   @param   loop_parameters_t  srcParam  parameters to be serialised
 *   @return  int  number of bytes written from packedData (the length of the serialised object)
 */
int PackParametersState(unsigned char *packedData, loop_parameters_t *srcParam);

/**
 *   \brief Unpacks Loop Parameter structure
 *
 *   Unpacks the serialisaed buffer to the struct gains_t.
 *   Uses the primitive_serialisation.h/c,
 *   Thanks to Joel.
 *
 *   @param   packingBuffer  unsigned char* buffer to which the serialised representation of the object will be stored
 *   @param   loop_parameters_t  paramDestination  parameters to be unserialised
 *   @return  int  number of bytes written from packedData (the length of the serialised object)
 */
int UnpackParametersState(const unsigned char *packedData, loop_parameters_t *paramDestination);


/******************** END - GCS related definitons - END *******************/
#endif

