/**
 * @file
 * @author Alex Wainwright
 *
 * Last Modified by: $Author: wainwright.alex $
 *
 * $LastChangedDate: 2009-10-17 16:00:35 +1000 (Sat, 17 Oct 2009) $
 * $Rev: 853 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 *
 * The AHNS Telemetry Thread is responsible for receiving telemetry data from the downlink. 
 *
 */

#include <QThread>
#include "ahns_autopilot.h"

typedef unsigned short uint_16;

/**
 * State enumeration for the telemetry thread state machine
 */
namespace telemetry{
enum telem_state
{
	S1,
	S2,
	S3,
	S4,
	S5,
	S6,
	S7,
	S8,
	S9,
};
}

/** 
 * QThread class responsible for receiving the telemetry. 
 * This thread runs constantly - r_thread::run() contains an infinite loop.  It reads telemetry data in through a serial port, maintains packet synchronisation, and calls the autopilot (ap_thread) with the telemetry data, every time it receives a new packet.
 */
class r_thread : public QThread
{
	Q_OBJECT;

public:
	r_thread(int port, ap_thread *ap_t, Telemetry *telem, op_mode *mode);
	void run();
	void stop();

signals:
	/// This signal is emitted every time the the the state machine receives a byte after having lost sync.  Effectively every time a byte is received out of synchronisaiton. 
	void lost_sync();
	/// This signal is emitted every time a packet is discarded due to the autopilot not being ready accept a new packet (i.e. it is still processing the previous packet.  The behaviour is such that when this occurs, the new packet is discarded, and the telemetry thread goes on to recieve the next packet, and attempts to start the autopilot again once it has. 
	void packet_discarded();

private:
	
	int telemetry_conversion15(uint_16 data, double *ret_val, double scale_factor);
	void emit_lost_sync();

	/// The port on which the thread receives telemetry data.
	int m_Port;

	/// A boolean variable indicating whether the thread has been requested to stop. 
	bool stopnow;

	/// A pointer to the autopilot thread which will be called to run after this thread successfully receives a packet
	ap_thread *autopilot_thread;

	Telemetry* telemetry;
	op_mode *ap_mode;
};

