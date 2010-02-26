/**
 * @file
 * @author Alex Wainwright
 *
 * Last Modified by: $Author: (local) $
 *
 * $LastChangedDate: 2010-02-24 20:27:56 +1000 (Wed, 24 Feb 2010) $
 * $Rev: 853M $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Source file for the telemetry code used by the autopilot
 */

#include <cstdio>
#include <stdint.h>
#include "ahns_telemetry.h"
#include "ahns_logger.h"


#define VERBOSE

//Sync bytes from telemetry
#define SYNC1 0xFF
#define SYNC2 0xFE
#define SYNC3 0xFD
#define SYNC4 0xFC

#define TELEMSIZE  22/// The number of bytes in the telemetry packet EXCLUDING sync bytes. (check with alex)

#define ACCEL_FACTOR 0.004062 /// unit conversion from mg to m/s/s
#define GYRO_FACTOR 0.0156 /// units of degrees/s

using namespace std;
using namespace telemetry;

/**
 * Initialisation of the telemetry receiver thread
 * @param port    The serial port to which the receiver is connected.
 * @param *ap_t   A pointer to the autopilot thread, which will be run once per packet received.
 * @param *telem  A pointer to the telemetry structure representing the UAV. See the Telemetry enumeration in ahns_telemetry.h.
 * @param *mode   A pointer to a variable descrobing the mode of the autopilot application - manual control (from sliders) or autonomous.
 */
r_thread::r_thread(int port, ap_thread *ap_t, Telemetry *telem, op_mode *mode)
{
	m_Port = port;
	autopilot_thread = ap_t;
	telemetry = telem;
	ap_mode = mode;
	stopnow = false;
}

/** 
 * Stops the telemetry thread from running by setting the stopnow variable.
 */
void r_thread::stop()
{
	AHNS_DEBUG("r_thread::stop()");
	stopnow = true;
	while(autopilot_thread->isRunning())
	{
		AHNS_DEBUG("r_thread::stop: waiting for autopilot thread to stop.");
		autopilot_thread->wait();
		usleep(10);
		AHNS_DEBUG("r_thread::stop: 	autopilot thread stopped!")
	}
	AHNS_DEBUG("r_thread::stop: [ COMPLETED SUCCESSFULLY ]");
}


/** 
 * Main receive thread - reads telemetry data from the serial port, then calls the autopilot thread once it has received an entire packet. Maintains packet synchronisation in accordance with ICD.
 */
void r_thread::run()
{
	AHNS_DEBUG("r_thread::run()");

	uint8_t *byte = new unsigned char;
	uint8_t buffer [TELEMSIZE];
	int buffer_index = 0;
	int ret;
	
	telem_state state = S1;
	
	while(!stopnow)
	{
#ifdef VERBOSE
		AHNS_DEBUG("r_thread::run:: reading a byte...");
#endif
		ret = read(m_Port, byte, 1);
#ifdef VERBOSE
		AHNS_DEBUG("r_thread::run:: read "<< ret << " byte(s).: ( " << setbase(16) << (int)*byte << " ).");
#endif
		if(ret)
		{
			// state machine
			switch (state)
			{
				case S1: // Lost synchronisation, looking for first sync
				{
					emit lost_sync();
					if (*byte == SYNC1)
					{
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run:: got sync1");
#endif
						state = S2;
					}
					else
					{
						state = S1;
					}
				}
				break;
				
				case S2: // Just got first sync, looking for second
				{
					if(*byte ==  SYNC2)
					{
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run:: got sync2");
#endif
						state = S3;
					}
					else if(*byte == SYNC1)
					{
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run:: got sync1 again when expecting sync 2");
#endif
						state = S2;
					}
					else
					{
						state = S1;
					}
				}
				break;
				
				case S3: // just got second sync byte, looking for third
				{
					if(*byte == SYNC3)
					{
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run:: got sync3");
#endif
						state = S4;
					}
					else
					{
						state = S1;
					}
				}
				break;
				
				case S4: // just got third sync, looking for fourth
				{
					if(*byte == SYNC4)
					{
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run:: got sync4");
#endif
						state = S5;
					}
					else
					{
						state = S1;
					}
				}
				break;
				
				case S5: // receiving data
				{
					//buffer the received byte
					buffer[buffer_index] = *byte;
					buffer_index++;
					
					if(buffer_index < TELEMSIZE)
					{
						state = S5;
					}
					else
					{
						state = S6;
					}
				}
				break;
				
				case S6: // just reached the end of the buffer, start checking next sync message where you'd expect it
				{
					if(*byte == SYNC1)
					{
						state = S7;
					}
					else // sync wasn't there, discard buffer and start looking for next sync message
					{
						buffer_index = 0;
						state = S1;
					}
				}
				break;
				
				case S7: // after filling buffer and getting first sync, check for second
				{
					if(*byte == SYNC2)
					{
						state = S8;
					}
					else //not the sync message, discard buffer
					{
						buffer_index = 0;
						state = S1;
					}
				}
				break;
				
				case S8: // after filling buffer and receiveng first two syncs, check for third
				{
					if(*byte == SYNC3)
					{
						state = S9;
					}
					else // not the sync message, discard buffer
					{
						buffer_index = 0;
						state = S1;
					}
				}
				break;
				
				case S9: // after filling buffer and getting three syncs, chech for fourth and final
				{
					if(*byte == SYNC4) // it was the sync message, store the buffer
					{
						//TODO: need to do a checksum!!!!!!!!1111one
						//TODO: MUTEX it up
						AHNS_DEBUG("r_thread::run: writing data from telemetry buffer to telemetry variable:\n");

/*						for (int ii=0;ii<TELEMSIZE;ii++)
						{
							AHNS_DEBUG(ii<< ": " << setbase(16) << (int)buffer[ii] << ", " << setbase(10));
						}
*/
						telemetry->time = (buffer[0]<<24 | buffer[1]<<16 | buffer[2]<<8 | buffer[3]);

						AHNS_DEBUG("r_thread::run: the time: (" << telemetry->time << " )");
						
						uint_16 t_x_accel = 	buffer[4]<<8  | buffer[5];
						uint_16 t_y_accel = 	buffer[6]<<8  | buffer[7];		
						uint_16 t_z_accel = 	buffer[8]<<8  | buffer[9];
						uint_16 t_roll_rate = 	buffer[10]<<8 | buffer[11];
						uint_16 t_pitch_rate = 	buffer[12]<<8 | buffer[13];
						uint_16 t_yaw_rate = 	buffer[14]<<8 | buffer[15];
						uint_16 t_alt = 	buffer[16]<<8 | buffer[17];
						uint_16 t_batt = 	buffer[18]<<8 | buffer[19];
						uint_16 t_crc = 	buffer[20]<<8 | buffer[21];

						AHNS_DEBUG("r_thread::run: the raw alt value: (" << t_alt << " )");
					
						uint_16 tempTelem;
	
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run: scaling x_accel.")
#endif						
						telemetry_conversion15(t_x_accel, &telemetry->x_accel, ACCEL_FACTOR);
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run: scaling y_accel.")
#endif						
						telemetry_conversion15(t_y_accel, &telemetry->y_accel, ACCEL_FACTOR);
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run: scaling z_accel.")
#endif						
						telemetry_conversion15(t_z_accel, &telemetry->z_accel, ACCEL_FACTOR);
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run: scaling roll_rate.")
#endif						
						telemetry_conversion15(t_roll_rate, &telemetry->roll_rate, GYRO_FACTOR);
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run: scaling pitch_rate.")
#endif						
						telemetry_conversion15(t_pitch_rate, &telemetry->pitch_rate, GYRO_FACTOR);
#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run: scaling yaw_rate.")
#endif						
						telemetry_conversion15(t_yaw_rate, &telemetry->yaw_rate, GYRO_FACTOR);
						telemetry->alt = t_alt; //converted to cm in autopilot
						telemetry->battery = 2.56*t_batt/1023.0 + 10.0;

#ifdef VERBOSE
						AHNS_DEBUG("r_thread::run: got whole packet, this is alt voltage: " << t_alt << " )");
						AHNS_DEBUG("r_thread::run: batter ROARRR ( " << t_batt << " ) ");
						AHNS_DEBUG("r_thread::run: this is the battery voltage: ( " << telemetry->battery << " )");
#endif
						

						AHNS_DEBUG("r_thread::run: Is it autonomous? ( " << (*ap_mode == autonomous) << " )");
						if(*ap_mode == autonomous)
						{
							AHNS_DEBUG("run: waiting for autopilot thread to finish (if still running)");
							//TODO: this waiting business is really bad. it's doing FIFO, we want FILO; really bad


//							autopilot_thread->wait(20); // wait up to 20ms for last autopilot iteration to finish
// TODO TODO TODO new method, wait for next packet if the autopilot thread is still running. --- this is still a shit way of doing it!!!!!!! TODO TODO TODO 
							AHNS_DEBUG("run: starting autopilot thread");
							if(!autopilot_thread->isRunning())
							{
								autopilot_thread->start(QThread::TimeCriticalPriority); //run the autopilot (it alread has the telemetry data)
							}
							else
							{
								emit packet_discarded();
							}
						}

						buffer_index = 0;
						
						state = S5;
					}
					else // it wasn't the sync message after all, discard buffer
					{
						buffer_index = 0;
						state = S1;
					}
				}
				break;
			}
		}
	}
	AHNS_DEBUG("r_thread::run: stopping, deleting 'byte'...");
	delete byte;
	AHNS_DEBUG("r_thread::run: deleted, returning.");
}

/**
 * Takes a 15-bit 2's compliment telemetry value, checks for validity (MSB is 0) and converts to binary.
 * @param data    The telemetry value.
 * @param retVal  A pointer to a uint_16 in which the answer is stored.
 * @return        0 If successful, -1 if the value was invalid and should be discarded.
 */
int r_thread::telemetry_conversion15(uint_16 data, double *ret_val, double scale_factor)
{
	if(data < 0x8000)
	{
		AHNS_DEBUG("r_thread::telemetry_conversion15: good data");
		
		if(data > 0x3FFF)
		{
			data += -1;
			*ret_val = -((~data) & 0x3FFF); 
			AHNS_DEBUG("r_thread::telemetry_conversion15: negative conversion ( " << *ret_val << " ).");
		}
		else
		{
			*ret_val = data;
			AHNS_DEBUG("r_thread::telemetry_conversion15: positive conversion ( " << *ret_val << " ).");
		}
		*ret_val = *ret_val * 2 * scale_factor; //multiply by 2 to increase to true value (16 bits).
		return 0;
	}
	else 
	{
		AHNS_DEBUG("r_thread::telemetry_conversion15: number discarded, using old value: ( " << *ret_val << " )");
		return -1;
	}
}

