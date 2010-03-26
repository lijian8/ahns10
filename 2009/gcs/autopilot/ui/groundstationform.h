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
 *
 * This file is the header file for the autopilot GUI.
 *
 */

#ifndef _IRIDIUMTESTCTRL_FORM_H_
#define _IRIDIUMTESTCTRL_FORM_H_

//status things 
#define OK 333
#define NO_PORT 334

#include <termios.h>
#include <fstream>

#include "ui_basegroundstation.h"
#include "ui_ahnsabout.h"

#include <QWidget>
#include <QString>
#include <QMap>
#include <QThread>
#include <QSlider>
#include <QDialog>
#include <QTimer>
#include <../QtNetwork/QUdpSocket>
#include <stdint.h>

#include "ahns_telemetry.h"
#include "hmi_packet.h"

class QCheckBox;

/**
 * The structure which contains all the variables to be recorded in the data log. 
 */
struct ahns_log_data
{
	double alt_command;
	double x_command;
	double y_command;
	double roll_command;
	double pitch_command;
	double yaw_command;
	
	double altitude;
	double roll;
	double pitch;
	double yaw;
	double x_pos;	
	double y_pos;
	double telem_time;

	double alt_p;
	double alt_i;
	double alt_d;
	double collective_cmd;

	double roll_p;
	double roll_i;
	double roll_d;
	
	double pitch_p;
	double pitch_i;
	double pitch_d;

	double yaw_p;
	double yaw_i;
	double yaw_d;

	double alt_p_gain;
	double alt_i_gain;
	double alt_d_gain;
	
	double roll_p_gain;
	double roll_i_gain;
	double roll_d_gain;
	
	double pitch_p_gain;
	double pitch_i_gain;
	double pitch_d_gain;

	double yaw_p_gain;
	double yaw_i_gain;
	double yaw_d_gain;

	double battery;
};

/**
 * A structure which holds data sent from the HMI.
*/
struct hmi_data
{	
	///Time since midnight in milliseconds
	double timestamp;	

	double joystick_x;
	double joystick_y;	/// The joystick values
	double joystick_z;

	/// Autoplito mode (full autonomous or stability augmentation)
	int    ap_mode;

	double ap_cmd_x;
	double ap_cmd_y;	/// Three commands for the autopilot
	double ap_cmd_alt;
};

/**
 * The about window class, inheriting from QWidget
 */
class AhnsAbout : public QWidget, private Ui::ahnsAbout
{
	Q_OBJECT
	
public:

	AhnsAbout(QWidget* parent = 0){setupUi(this);}
	~AhnsAbout(){}
	
private slots:

};

/**
 * A class controlling what is displayed on the main form's status bar.
 */
class ahnsStatus
{
	public:		
		ahnsStatus();

		int printStatus(char* buff, int buffMax);
		
		void setTelemetry(int status);
		int getTelemetry();
		void setGroundpulse(int status);
		int getGroundpulse();
		void setHMI(int status);
		int getHMI();

	private:
		int telemetry;
		int groundpulse;
		int hmi;
};

/**
 * The main window class, inheriting from QMainWindow
 */
class GroundStationForm : public QMainWindow, private Ui::BaseGroundStation
{
	Q_OBJECT

public:
	
	GroundStationForm(QWidget* parent = 0);
	~GroundStationForm(){} 
		
	op_mode ap_mode;
		
private slots:

	// Button slots
	void About();
	void on_closeBtn_clicked();
	void on_setGainsBtn_clicked();
	void on_saveGainsBtn_clicked();
	void on_setTrimsBtn_clicked();
	void on_saveTrimsBtn_clicked();
	void on_manualRad_clicked();	
	void on_autonomousRad_clicked();

	void on_latZeroBtn_clicked();
	void on_lonZeroBtn_clicked();
	void on_colZeroBtn_clicked();
	void on_rudZeroBtn_clicked();

	void on_setCmdsBtn_clicked();
	void on_altCmdResetBtn_clicked();
	void on_rollCmdResetBtn_clicked();
	void on_pitchCmdResetBtn_clicked();
	void on_yawCmdResetBtn_clicked();

	void set_state_bars();
	void set_output_bars();
	void slider_send();
	void lost_sync_counter();
	void discarded_packet_counter();

	void hmi_receive();
	void hmi_send();
	
private:

	int groundpulse_init();
	int telemetry_init();
	int read_gains();
	int read_trims();

	void updateStatus();

	void groundpulse_good();
	void groundpulse_bad();
	void telemetry_good();
	void telemetry_bad();
	
	void initialise_datalog();
	void zero_logdata();
	void log_data();
	void close_datalog();

	void closeEvent(QCloseEvent *event);

	void hmi_rx_initialise();
	void hmi_rx_machine(uint8_t* byte);
	void hmi_display();

	void hmi_tx_initialise();

	ahns_log_data logdata;

	AhnsAbout* abt;
	int telemetry_port;
	int groundpulse_port;	
	Telemetry* emily_telem; 

	QTimer *slider_timer;
	QTime *sys_rate;	
	QTimer *hmi_send_timer;

	QUdpSocket *hmi_rx_sock;
	QUdpSocket *hmi_tx_sock;
	
	r_thread *telemetry_thread;
	ap_thread *emily_ap;

	std::fstream gains;
	std::fstream trims;
	std::fstream datalog;

	uplink::heli_trims emily_trim;

	ahnsStatus theStatus;
	uint8_t tgStatus;

	hmi_data hmi_packet;

};



#endif

