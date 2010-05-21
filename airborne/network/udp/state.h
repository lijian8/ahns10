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

/******************************************************************************/
/****************** START - iphone related definitons - START *****************/
/******************************************************************************/

/**
 *	\brief Heli Control input float - input value described as a float and declaring its own validity
 */
typedef struct {
	float 	value;		/**< heli control input described as a floating point number */
	int8_t	isValid;	/**< equates to true if the value is valid, false if the value is invalid */
} iphone_input_float_t;

/**
 *	\brief Heli Control primary input sample
 *	
 *	This data structure represents a sampling of the state of each of the 
 *	primary input controllers used to control the helicopter via the iphone, at
 *	a particular point in time
 */
typedef struct {
	
	/** @name Sampled input values of the primary controllers */
	iphone_input_float_t	lateralIntensity;			/**< lateral cyclic controller, normalised -1..+1 */
	iphone_input_float_t	longitudinalIntensity;		/**< longitudinal cyclic controller, normalised -1..+1 */
	iphone_input_float_t	yawIntensity;				/**< yaw controller, normalised -1..+1 */
	iphone_input_float_t	collectiveLiftIntensity;	/**< collective controller, causes the aircraft to lift and descend when subjected to gravity, normalised 0..+1 */
	
	/** @name Details relating to the sample */
	double		timestamp;			/**< time at which the sample was taken, expressed in seconds (including fractions of a second) since UNIX epoch */
	int8_t		wasControllerPaused;	/**< equates to true if the controllers were 'paused' while this sample was recorded, else false */
} iphone_primary_input_sample_t;


#define IPHONE_PRIMARY_INPUT_SAMPLE_T_SERIALISED_SIZE 29 // Octects (this can differ from sizeof(iphone_primary_input_sample_t) due to padding)

/**
 *	Prints a version of the contents of the sample to the designated output buffer
 *
 *	@param	sample	iphone_primary_input_sample_t	the contents of this sample get written to the buffer
 *	@param	indentationDegree	int					number of tab chars to prepend to each line of the output
 *	@param	outputStream		FILE*				output stream to write the sample description to.  (ie. stdout, stderr)
 *	
 *	@return	void
 */
void PrintIphonePrimarySample(iphone_primary_input_sample_t sample, int indentationDegree, FILE *outputStream);

/**
 *	Determines if packedData is a valid packed (serialised) representation of an iphone_primary_input_sample_t object
 *
 *	Currently it just makes sure packedData is of the correct length.
 *
 *	@param	packedData			void*	pointer to the data representing a serialised iphone_primary_input_sample_t object
 *	@param	packedDataLength	int		the length of packedData in bytes
 *	@return						int		true if packedData is valid, else false
 */
int ValidatePackedIphonePrimaryInputSample(const void *packedData, int packedDataLength);

/**
 *	Unpacks (unserialises) an iphone_primary_input_sample_t object and guarantees it is in host byte-order
 *
 *	@param	packedData			unsigned char*					pointer to the serialised representation of the object to unpack
 *	@param	smplDestination		iphone_primary_input_sample_t*	destination for the resultant unpacked iphone_primary_input_sample_t object
 *
 *	@return						int								number of bytes read from packedData (the length of the serialised object)
 */
int UnpackIphonePrimaryInputSample(const unsigned char *packedData, iphone_primary_input_sample_t *smplDestination);

/**
 *	Packs (serialises) an iphone_primary_input_sample_t object into the packing buffer and guarantees it is in network byte-order
 *
 *	Unpacks (unserialises) an iphone_primary_input_sample_t object and guarantees it is in host byte-order.
 *	Currently the serialised respresentation of iphone_primary_input_sample_t objects, is usually smaller than sizeof(iphone_primary_input_sample_t)
 *	due to padding of structs, which isn't recorded when the struct is serialised.
 *
 *	@param	packingBuffer	unsigned char*					buffer to which the serialised representation of the object will be stored
 *	@param	smplSource		iphone_primary_input_sample_t	sample to be serialised
 *
 *	@return					int								number of bytes written from packedData (the length of the serialised object)
 */
int PackIphonePrimaryInputSample(unsigned char *packingBuffer, iphone_primary_input_sample_t smplSource);


/******************** END - iphone related definitons - END *******************/


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



/******************** END - GCS related definitons - END *******************/
#endif

