/**
 * @file
 * @author Alex Wainwright
 *
 * Last Modified by: $Author: wainwright.alex $ 
 *
 * $LastChangedDate: 2009-10-18 00:05:16 +1000 (Sun, 18 Oct 2009) $
 * $Rev: 860 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 *
 * This file is the main source file for the autopilot GUI, and the entire autopilot application is based around this file. 
 *
 */

#include "groundstationform.h"

#include <QString>
#include <QDateTime>
#include <QColor>
#include <QLCDNumber>
#include <QTime>
#include <QTimer>
#include <QStatusBar>
#include <qradiobutton.h>

#include <sys/fcntl.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <stdint.h>
#include <time.h>

#include "ahns_logger.h"
#include "pid_class.h"
#include "uplink.h"
#include "ap_packet.h"

#define GAINS_FILE "gains.ahns"	///Name of the file used to store gains
#define TRIMS_FILE "trims.ahns"	///Name of the file used to store trims
#define N_GAINS 12		///Number of gains stored in the gains file 
#define N_TRIMS 12		///Number of trims/bounds stored in the trims file

#define TELEM_BAUD B115200
#define GROUNDPULSE_BAUD B19200
#define HMI_RX_PORT 45454
#define HMI_TX_PORT 45455
#define HMI_IP "192.168.0.13"

using namespace std;
using namespace uplink;

/**
 * Initialises the ground station form 
 */
GroundStationForm::GroundStationForm(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);
	
	ahns_logger_init("autopilot.log");
	
	AHNS_DEBUG("GroundStationForm::GroundStationForm()");

	initialise_datalog();

	zero_logdata();

	slider_timer = new QTimer(this);
	hmi_send_timer = new QTimer(this);
	sys_rate = new QTime(0,0,0,0);

	if(groundpulse_init()==0)
	{
		slider_timer->start(20);
		groundpulse_good();
		theStatus.setGroundpulse(OK);
		updateStatus();
	}
	else
	{
		AHNS_ALERT("GroundStationForm:: Groundpulse connection initialisation failed. Please restart the application.");
		groundpulse_bad();
		theStatus.setGroundpulse(NO_PORT);
		updateStatus();
	}

	telemetry_thread = NULL;
	emily_ap = NULL;
	emily_telem = NULL;
	hmi_rx_sock = NULL;

	hmi_rx_initialise();
	hmi_tx_initialise();

	// Initialise the telemetry and autopilot threads
	emily_telem = new Telemetry;
	emily_ap = new ap_thread(emily_telem, groundpulse_port, &emily_trim);	
	
	if (autonomousRad->isChecked())
	{
		ap_mode = autonomous;
	}
	else
	{
		ap_mode = manual;
	}

	char time[20];
	sprintf(time,"%02i:%02i . %02i%c",0,0,0,'\0');
	telemTimeLcd->display(time);


	if(read_gains()!=0)
	{
		AHNS_ALERT("GroundStationForm:: Gain failed.");
	}

	if(read_trims()!=0)
	{
		AHNS_ALERT("GroundStationForm:: Trim configuration failed.");
	}

	if(telemetry_init()==0)
	{
		AHNS_DEBUG("GroundStationForm:: Telemetry port initialised.");
		telemetry_thread = new r_thread(telemetry_port, emily_ap, emily_telem, &ap_mode);
		telemetry_thread->start(QThread::HighestPriority);
		telemetry_good();
		theStatus.setTelemetry(OK);
		updateStatus();
	}
	else
	{
		AHNS_ALERT("GroundStationForm:: Telemetry initialisation failed. Please restart the application.");
		telemetry_bad();
		theStatus.setTelemetry(NO_PORT);
		updateStatus();
	}
	

	AHNS_DEBUG("GroundStationForm:: connecting signals to slots.");
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(About()));
        connect(actionExit,SIGNAL(triggered()),this,SLOT(close()));
        connect(slider_timer, SIGNAL(timeout()), this, SLOT(slider_send()));
	connect(hmi_send_timer, SIGNAL(timeout()), this, SLOT(hmi_send()));
	connect(altFilLenBox, SIGNAL(valueChanged(int)), emily_ap, SLOT(setAltFiltLength(int)));
	connect(rateFilLenBox, SIGNAL(valueChanged(int)), emily_ap, SLOT(setPeriodFiltLength(int)));
	connect(battFilLenBox, SIGNAL(valueChanged(int)), emily_ap, SLOT(setBattFiltLength(int)));
	connect(telemetry_thread, SIGNAL(lost_sync()), this, SLOT(lost_sync_counter()));
	connect(telemetry_thread, SIGNAL(packet_discarded()), this, SLOT(discarded_packet_counter()));
	AHNS_DEBUG("GroundStationForm:: connecting UPD signal and slot.");
	connect(hmi_rx_sock, SIGNAL(readyRead()), this, SLOT(hmi_receive()));
	AHNS_DEBUG("GroundStationForm:: state signal...");
	connect(emily_ap, SIGNAL(new_state()), this, SLOT(set_state_bars()));
	AHNS_DEBUG("GroundStationForm:: command signal...");
	connect(emily_ap, SIGNAL(new_cmd()), this, SLOT(set_output_bars()));
	AHNS_DEBUG("GroundStationForm:: all connected.");
	updateStatus();

	on_setCmdsBtn_clicked();
	battFilLenBox->setValue(400);

	hmi_send_timer->start(40);

	abt = new AhnsAbout();
	AHNS_DEBUG("GroundStationForm:: completed initialisation.");
}

/**
 * Sets all the log variables to zero
 */
void GroundStationForm::zero_logdata()
{
	logdata.alt_command = 0;
	logdata.x_command = 0;
	logdata.y_command = 0;
	logdata.roll_command = 0;	
	logdata.pitch_command = 0;
	logdata.yaw_command = 0;

	logdata.altitude = 0;
	logdata.roll = 0;
	logdata.pitch = 0;
	logdata.yaw = 0;
	logdata.x_pos = 0;
	logdata.y_pos = 0;
	logdata.telem_time= 0;
	
	logdata.alt_p = 0;
	logdata.alt_i = 0;
	logdata.alt_d = 0;
	
	logdata.roll_p = 0;
	logdata.roll_i = 0;
	logdata.roll_d = 0;

	logdata.yaw_p = 0;
	logdata.yaw_i = 0;
	logdata.yaw_d = 0;

	logdata.alt_p_gain = 0;
	logdata.alt_i_gain = 0;	
	logdata.alt_d_gain = 0;
	
	logdata.roll_p_gain = 0;
	logdata.roll_i_gain = 0;	
	logdata.roll_d_gain = 0;
	
	logdata.pitch_p_gain = 0;
	logdata.pitch_i_gain = 0;
	logdata.pitch_d_gain = 0;

	logdata.yaw_p_gain = 0;
	logdata.yaw_i_gain = 0;
	logdata.yaw_d_gain = 0;
}

/**
 * Initialises the serial port for output to groundpulse
 */
int GroundStationForm::groundpulse_init()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime (buffer, 80, "%Y-%m-%d_%H-%M.log", timeinfo);
	puts(buffer);

	//TODO: this is copied from an old version of ap-interface and probably needs replacing

	AHNS_DEBUG("GroundStationForm::groundpulse_init()");

	groundpulse_port = open("/dev/ttyUSB0", O_RDWR);
	if(groundpulse_port < 0)
	{
		AHNS_ALERT("groundpulse_init: could not open serial port. open returned");	
		return -1;
	}

	AHNS_DEBUG("groundpulse_init: serial port opened.");
	
	termios settings;

	settings.c_iflag = (BRKINT|IGNPAR|INPCK|IMAXBEL);
	settings.c_cflag = (GROUNDPULSE_BAUD|CLOCAL|CREAD|PARENB|PARODD|CS8);
	settings.c_lflag = 0;
	settings.c_oflag = 0;
	settings.c_cc[VTIME] = 0;
	settings.c_cc[VMIN] = 1;

	//TODO: look into the read settings, namely c_cc[VTIME] c_cc[VMIN]  etc....
	
	tcflush(groundpulse_port, TCIFLUSH);
	AHNS_DEBUG("groundpulse_init: serial port flushed.");

	if(tcsetattr(groundpulse_port,TCSANOW,&settings)<0)
	{
		AHNS_ALERT("groundpulse_init: could not set serial port attributes.");
		return -1;
	}

	return 0;
}

/**
 * Initialises the serial port to read telemetry
 */
int GroundStationForm::telemetry_init()
{
	//TODO: this is copied from an old version of ap-interface and probably needs replacing

	AHNS_DEBUG("GroundStationForm::telemetry_init()");

	telemetry_port = open("/dev/ttyS0", O_RDWR);
	if(telemetry_port < 0)
	{
		AHNS_ALERT("telemetry_init: could not open serial port. open returned");	
		return -1;
	}

	AHNS_DEBUG("telemetry_init: serial port opened.");
	
	termios settings;

	settings.c_iflag = (BRKINT|IGNPAR|INPCK|IMAXBEL);
	settings.c_cflag = (TELEM_BAUD|CLOCAL|CREAD|/*PARENB|PARODD|*/CS8);
	settings.c_lflag = 0;
	settings.c_oflag = 0;
	settings.c_cc[VTIME] = 1;
	settings.c_cc[VMIN] = 0;

	//TODO: look into the read settings, namely c_cc[VTIME] c_cc[VMIN]  etc....
	
	tcflush(telemetry_port, TCIFLUSH);
	AHNS_DEBUG("telemetry_init: serial port flushed.");

	if(tcsetattr(telemetry_port,TCSANOW,&settings)<0)
	{
		AHNS_ALERT("telemetry_init: could not set serial port attributes.");
		return -1;
	}

	return 0;
}

/**
 * Read the autopilot gains file.
 */
int GroundStationForm::read_gains()
{
	AHNS_DEBUG("GroundStationForm::read_gains()");
	double buffer[12];
	int ii = 0;
	gains.open(GAINS_FILE, fstream::in);
	
	while(!gains.eof() && !gains.fail() && ii<N_GAINS)
	{
		gains >> buffer[ii];
		ii++;
	}
	if(ii!=N_GAINS)
	{
		AHNS_ALERT("read_gains:: could not read all ( " << N_GAINS << " ) gains from config file.");
		if(gains.eof())
		{
			AHNS_ALERT("read_gains:: \treached end of file.");
		}
		if(gains.fail())
		{
			AHNS_ALERT("read_gains:: \tread failed - check gains.ahns is correct.");
		}

		return -1;
	}
	else
	{
		PitchPBox->setValue(buffer[0]);
		PitchIBox->setValue(buffer[1]);
		PitchDBox->setValue(buffer[2]);
		RollPBox->setValue(buffer[3]);
		RollIBox->setValue(buffer[4]);
		RollDBox->setValue(buffer[5]);
		YawPBox->setValue(buffer[6]);
		YawIBox->setValue(buffer[7]);
		YawDBox->setValue(buffer[8]);
		ColPBox->setValue(buffer[9]);
		ColIBox->setValue(buffer[10]);
		ColDBox->setValue(buffer[11]);

		//set the log variables
		logdata.pitch_p_gain = buffer[0];
		logdata.pitch_i_gain = buffer[1];
		logdata.pitch_d_gain = buffer[2];
		logdata.roll_p_gain = buffer[3];
		logdata.roll_i_gain = buffer[4];
		logdata.roll_d_gain = buffer[5];
		logdata.yaw_p_gain = buffer[6];
		logdata.yaw_i_gain = buffer[7];
		logdata.yaw_d_gain = buffer[8];
		logdata.alt_p_gain = buffer[9];
		logdata.alt_i_gain = buffer[10];
		logdata.alt_d_gain = buffer[11];
		
		//log the data
		log_data();

		on_setGainsBtn_clicked();

		AHNS_DEBUG("read_gains:: completed successfully.");

		return 0;
	}
}

/** 
 * Read the autopilot trims file.
 */
int GroundStationForm::read_trims()
{
	AHNS_DEBUG("GroundStationForm::read_trims()");
	double buffer[N_TRIMS];
	int ii = 0;
	trims.open(TRIMS_FILE, fstream::in);
	
	while(!trims.eof() && !trims.fail() && ii<N_TRIMS)
	{
		trims >> buffer[ii];
		ii++;
	}
	if(ii!=N_TRIMS)	//Didn't read the right number of trims.
	{
		AHNS_ALERT("read_trims:: could not read all ( " << N_TRIMS << " ) trims from config file.");
		if(trims.eof())
		{
			AHNS_ALERT("read_trims:: \treached end of file.");
		}
		if(trims.fail())
		{
			AHNS_ALERT("read_trims:: \tread failed - check trims.ahns is correct.");
		}

		return -1;
	}
	else	// Read the right trims, set them.
	{
		PitchTrimBox->setValue(buffer[0]);
		RollTrimBox->setValue(buffer[1]);
		YawTrimBox->setValue(buffer[2]);
		ColTrimBox->setValue(buffer[3]);

		PitchMaxBox->setValue(buffer[4]);
		RollMaxBox->setValue(buffer[5]);
		YawMaxBox->setValue(buffer[6]);
		ColMaxBox->setValue(buffer[7]);

		PitchMinBox->setValue(buffer[8]);
		RollMinBox->setValue(buffer[9]);
		YawMinBox->setValue(buffer[10]);
		ColMinBox->setValue(buffer[11]);

		on_setTrimsBtn_clicked();

		AHNS_DEBUG("read_trims:: completed successfully !!!!!111one");

		return 0;
	}
}

/**
 * Opens the About window for the autopilot
 */
void GroundStationForm::About()
{
	abt->show();
}

/**
 * Sets the gains by what is in the gain boxes on the GUI.
 */
void GroundStationForm::on_setGainsBtn_clicked()
{
	double *pitch = new double[3];
	double *roll = new double[3];
	double *yaw = new double[3];
	double *collective = new double[3];

	// Read in all the values from the spin boxes
	pitch[0] = PitchPBox->value();
	pitch[1] = PitchIBox->value();
	pitch[2] = PitchDBox->value();
	roll[0] = RollPBox->value();
	roll[1] = RollIBox->value();
	roll[2] = RollDBox->value();
	yaw[0] = YawPBox->value();
	yaw[1] = YawIBox->value();
	yaw[2] = YawDBox->value();
	collective[0] = ColPBox->value();
	collective[1] = ColIBox->value();
	collective[2] = ColDBox->value();

	// Set the gains in the autopilot using the setGains function
	emily_ap->setGains(pitch, roll, yaw, collective);

	// Set the log data
	logdata.pitch_p_gain = PitchPBox->value();
	logdata.pitch_i_gain = PitchPBox->value();
	logdata.pitch_d_gain = PitchDBox->value();
	logdata.roll_p_gain = RollPBox->value();
	logdata.roll_i_gain = RollIBox->value();
	logdata.roll_d_gain = RollDBox->value();
	logdata.yaw_p_gain = YawPBox->value();
	logdata.yaw_i_gain = YawIBox->value();
	logdata.yaw_d_gain = YawDBox->value();	
	logdata.alt_p_gain = ColPBox->value();
	logdata.alt_i_gain = ColIBox->value();
	logdata.alt_d_gain = ColDBox->value();

	delete []pitch;
	delete []roll;
	delete []yaw;
	delete []collective;
}

/**
 * Saves the gains to file (gains.ahns) which is read at start up. 
 */
void GroundStationForm::on_saveGainsBtn_clicked()
{
	AHNS_DEBUG("GroundStationForm::on_saveGainsBtn_clicked()");
	double pitch[3];
	double roll[3];
	double yaw[3];
	double collective[3];
	int ii;
	
	if(gains.is_open())
	{
		gains.close();
	}

	gains.open(GAINS_FILE, fstream::out|fstream::trunc);

	pitch[0] = PitchPBox->value();
	pitch[1] = PitchIBox->value();
	pitch[2] = PitchDBox->value();
	roll[0] = RollPBox->value();
	roll[1] = RollIBox->value();
	roll[2] = RollDBox->value();
	yaw[0] = YawPBox->value();
	yaw[1] = YawIBox->value();
	yaw[2] = YawDBox->value();
	collective[0] = ColPBox->value();
	collective[1] = ColIBox->value();
	collective[2] = ColDBox->value();

	for(ii=0; ii<3; ii++)
	{
		gains << pitch[ii] << '\t';
	}
	gains << endl;

	for(ii=0; ii<3; ii++)
	{
		gains << roll[ii] << '\t';
	}
	gains << endl;

	for(ii=0; ii<3; ii++)
	{
		gains << yaw[ii] << '\t';
	}
	gains << endl;

	for(ii=0; ii<3; ii++)
	{
		gains << collective[ii] << '\t';
	}
	gains << endl;

	AHNS_DEBUG("on_saveGainsBtn_clicked:: Gains saved");

}
/**
 * Sets the trims by the values in the boxes on the GUI.
 */
void GroundStationForm::on_setTrimsBtn_clicked()
{
	AHNS_DEBUG("GroundStationForm::on_setTrimsBtn_clicked()");
	emily_trim.roll = RollTrimBox->value();
	emily_trim.pitch = PitchTrimBox->value();
	emily_trim.rudder = YawTrimBox->value();
	emily_trim.collective = ColTrimBox->value();
	emily_ap->setBounds(PitchMaxBox->value(), PitchMinBox->value(), RollMaxBox->value(), RollMinBox->value(), ColMaxBox->value(), ColMinBox->value(), YawMaxBox->value(), YawMinBox->value());
	AHNS_DEBUG("on_setTrimsBtn_clicked:: Trims like... totally set.");
	///TODO: Add logging. This will require changing the ahns_log_data structure, and both the logging initialisation function and the logging function.
}

/**
 * Save the trims to file (trims.ahns) which is read at startup.
 */
void GroundStationForm::on_saveTrimsBtn_clicked()
{
	AHNS_DEBUG("GroundStationForm::on_saveTrimsBtn_clicked()");
	double trim[N_TRIMS];
	int ii;
	
	if(trims.is_open())
	{
		trims.close();
	}

	trims.open(TRIMS_FILE, fstream::out|fstream::trunc);

	trim[0] = PitchTrimBox->value();
	trim[1] = RollTrimBox->value();
	trim[2] = YawTrimBox->value();
	trim[3] = ColTrimBox->value();	

	trim[4] = PitchMaxBox->value();
	trim[5] = RollMaxBox->value();
	trim[6] = YawMaxBox->value();
	trim[7] = ColMaxBox->value();

	trim[8] = PitchMinBox->value();
	trim[9] = RollMinBox->value();
	trim[10] = YawMinBox->value();
	trim[11] = ColMinBox->value();

	for(ii=0; ii<N_TRIMS; ii++)
	{
		trims << trim[ii] << '\n';
	}
	gains << endl;
	AHNS_DEBUG("on_saveTrimsBtn_clicked:: Trims saved.");
}

/**
 * The slot for when the radio button selecets manual mode. 
 */
void GroundStationForm::on_manualRad_clicked()
{
	if(ap_mode == autonomous)
	{
		ap_mode = manual;
		slider_timer->start(20);
		AHNS_DEBUG("GroundStationForm:: Manual Mode.");
	}
}

/**
 * The slot for when the radio button selects autonomous mode.
 */
void GroundStationForm::on_autonomousRad_clicked()
{
	if(ap_mode == manual)
	{
		slider_timer->stop();
		ap_mode = autonomous;
		AHNS_DEBUG("GroundStationForm:: Autonomous Mode.");
	}
}

/**
 * Sets the manual lateral control to zero.
 */
void GroundStationForm::on_latZeroBtn_clicked()
{
	lateralSlider->setSliderPosition(0);
}

/**
 * Sets the manual longitudinal control to zero.
 */
void GroundStationForm::on_lonZeroBtn_clicked()
{
	longitudinalSlider->setSliderPosition(0);
}

/**
 * Sets the manual collective control to zero.
 */
void GroundStationForm::on_colZeroBtn_clicked()
{
	collectiveSlider->setSliderPosition(0);
}

/**
 * Sets the manual rudder control to zero.
 */
void GroundStationForm::on_rudZeroBtn_clicked()
{
	rudderSlider->setSliderPosition(0);
}

/**
 * Reads the command values from the GUI spin boxes and sends them to the autopilot.
 */
void GroundStationForm::on_setCmdsBtn_clicked()
{
	//the autopilot is currently running in centimetres. these are in metres. inconsistent units are the bane of my existence. just ask adam.
	emily_ap->setCommands(100*pitchCmdBox->value(), 100*rollCmdBox->value(), 100*yawCmdBox->value(), 100*altCmdBox->value());

	// Set the log variables
	logdata.pitch_command = pitchCmdBox->value();
	logdata.roll_command = rollCmdBox->value();
	logdata.yaw_command = yawCmdBox->value();
	logdata.alt_command = altCmdBox->value();
}

/**
 * Resets the altitude command to 1.5 and sends immediately to the autopilot.
 */
void GroundStationForm::on_altCmdResetBtn_clicked()
{
	altCmdBox->setValue(1.5);
	on_setCmdsBtn_clicked();
}

/**
 * Resets the roll command to 0 and sends immediately to the autopilot.
 */
void GroundStationForm::on_rollCmdResetBtn_clicked()
{
	rollCmdBox->setValue(0.0);
	on_setCmdsBtn_clicked();
}

/**
 * Resets the pitch command to 0 and sends immediately to the autopilot.
 */
void GroundStationForm::on_pitchCmdResetBtn_clicked()
{
	pitchCmdBox->setValue(0.0);
	on_setCmdsBtn_clicked();
}

/**
 * Resets the yaw command to 0 and sends immediately to the autopilot.
 */
void GroundStationForm::on_yawCmdResetBtn_clicked()
{
	yawCmdBox->setValue(0.0);
	on_setCmdsBtn_clicked();
}

/**
 * Changes the colour of a box on the GUI indicating whether the groundpulse port is open.
 */
void GroundStationForm::groundpulse_good()
{
	QPalette Pal(groundpulseLbl->palette());
	Pal.setColor(QPalette::Window, Qt::green);
	groundpulseLbl->setPalette(Pal);
}

/**
 * Changes the colour of a box on the GUI indicating whether the groundpulse port is open.
 */
void GroundStationForm::groundpulse_bad()
{
	QPalette Pal(groundpulseLbl->palette());
	Pal.setColor(QPalette::Window, Qt::red);
	groundpulseLbl->setPalette(Pal);
}


/**
 * Changes the colour of a box on the GUI indicating whether the telemetry port is open.
 */
void GroundStationForm::telemetry_good()
{
	QPalette Pal(telemetryLbl->palette());
	Pal.setColor(QPalette::Window, Qt::green);
	telemetryLbl->setPalette(Pal);
}

/**
 * Changes the colour of a box on the GUI indicating whether the telemetry port is open.
 */
void GroundStationForm::telemetry_bad()
{
	QPalette Pal(telemetryLbl->palette());
	Pal.setColor(QPalette::Window, Qt::red);
	telemetryLbl->setPalette(Pal);
}

/**
 * Sets the state bars and other related displays on the Autopilot GUI.
 */
void GroundStationForm::set_state_bars()
{
	AHNS_DEBUG("GroundStationForm::set_state_bars()");
	State temp_state = emily_ap->get_state();

	packetCountLcd->display(packetCountLcd->intValue()+1);

	// Set the log variables
	logdata.pitch = temp_state.pitch;
	logdata.roll = temp_state.roll;
	logdata.yaw = temp_state.yaw;
	logdata.x_pos = temp_state.x;
	logdata.y_pos = temp_state.y;
	logdata.altitude = temp_state.z;
	logdata.telem_time = temp_state.time;
	logdata.battery = temp_state.battery;
	
	if(temp_state.pitch > pitchBar->maximum())
		temp_state.pitch = pitchBar->maximum();
	if(temp_state.pitch < pitchBar->minimum())
		temp_state.pitch = pitchBar->minimum();
	pitchBar->setValue(temp_state.pitch);

	if(temp_state.roll > rollBar->maximum())
		temp_state.roll = rollBar->maximum();
	if(temp_state.roll < rollBar->minimum())
		temp_state.roll = rollBar->minimum();
	rollBar->setValue(temp_state.roll);

	if(temp_state.yaw > yawBar->maximum())
		temp_state.yaw = yawBar->maximum();	
	if(temp_state.yaw < yawBar->minimum())
		temp_state.yaw = yawBar->minimum();
	yawBar->setValue(temp_state.yaw);

	if(temp_state.x > xBar->maximum())
		temp_state.x = xBar->maximum();
	if(temp_state.x < xBar->minimum())
		temp_state.x = xBar->minimum();
	xBar->setValue(temp_state.x);

	if(temp_state.y > yBar->maximum())
		temp_state.y = yBar->maximum();
	if(temp_state.y < yBar->minimum())
		temp_state.y = yBar->minimum();
	yBar->setValue(temp_state.y);

	if(temp_state.z > altBar->maximum())
		temp_state.z = altBar->maximum();
	if(temp_state.z < altBar->minimum())
		temp_state.z = altBar->minimum();
	altBar->setValue(temp_state.z);

	if(temp_state.battery > batteryBar->maximum())
		temp_state.battery = batteryBar->maximum();
	if(temp_state.battery < batteryBar->minimum())
		temp_state.battery = batteryBar->minimum();
	batteryBar->setValue(temp_state.battery);

	int minutes = (int)(temp_state.time/60);
	int seconds = (int) (temp_state.time - 60*minutes);
	AHNS_DEBUG("set_state_bars:: temp_state.time : ( " << setbase(10) << temp_state.time << " ) and (int)(temp_state.time + 0.5): ( " << (int)(temp_state.time + 0.5) << " )");
	int millisecs = (int)(1000*(temp_state.time - (int)(temp_state.time)));
	int hundredths = (int)((millisecs)/10.0);
	char time[20];

//	cerr << "The seconds: ( " << seconds << " )" << endl;

	AHNS_DEBUG("set_state_bars:: mins, secs, milli: ( " << setbase(10) << minutes << ", " << seconds << ", " << millisecs << " )");
	
	sprintf(time,"%02i:%02i . %02i%c",minutes,seconds,hundredths,'\0');
	telemTimeLcd->display(time);
	packetRateBar->setValue(1.0/temp_state.period);

}

/**
 * Sets the ouput bars on the Autopilot GUI.
 */
void GroundStationForm::set_output_bars()
{
//	sys_rate->elapsed();
	double latPID[3];
	double lonPID[3];
	double colPID[3];
	double rudPID[3];
	int latSat, lonSat, colSat, rudSat;

	heli_command temp_cmd = emily_ap->get_cmd();
	emily_ap->getPIDs(latPID, lonPID, colPID, rudPID);
	emily_ap->querySaturation(&latSat, &lonSat, &colSat, &rudSat);
	
	// Set the log variables
	logdata.roll_p = latPID[0];
	logdata.roll_i = latPID[1];
	logdata.roll_d = latPID[2];
	logdata.pitch_p = lonPID[0];
	logdata.pitch_i = lonPID[1];
	logdata.pitch_d = lonPID[2];
	logdata.yaw_p = rudPID[0];
	logdata.yaw_i = rudPID[1];
	logdata.yaw_d = rudPID[2];
	logdata.alt_p = colPID[0];
	logdata.alt_i = colPID[1];
	logdata.alt_d = colPID[2];

	logdata.collective_cmd = temp_cmd.collective;
	
	QPalette latPal(latSatLbl->palette());
	switch (latSat)
	{
		case SATURATED_HIGH:
		{
			latPal.setColor(QPalette::Window, Qt::red);
			latSatLbl->setPalette(latPal);
		}
		break;
		case SATURATED_LOW:
		{
			latPal.setColor(QPalette::Window, Qt::black);
			latSatLbl->setPalette(latPal);
		}
		break;
		case NOT_SATURATED:
		{
			latPal.setColor(QPalette::Window, Qt::green);
			latSatLbl->setPalette(latPal);
		}
		break;
		default:
		{
			AHNS_ERROR("GroundStationForm::set_output_bars: invalid saturation state.");
		}
	}

	QPalette lonPal(lonSatLbl->palette());
	switch (lonSat)
	{
		case SATURATED_HIGH:
		{
			lonPal.setColor(QPalette::Window, Qt::red);
			lonSatLbl->setPalette(lonPal);
		}
		break;
		case SATURATED_LOW:
		{
			lonPal.setColor(QPalette::Window, Qt::black);
			lonSatLbl->setPalette(lonPal);
		}
		break;
		case NOT_SATURATED:
		{
			lonPal.setColor(QPalette::Window, Qt::green);
			lonSatLbl->setPalette(lonPal);
		}
		break;
		default:
		{
			AHNS_ERROR("GroundStationForm::set_output_bars: invalid saturation state.");
		}
	}

	QPalette colPal(colSatLbl->palette());
	switch (colSat)
	{
		case SATURATED_HIGH:
		{
			colPal.setColor(QPalette::Window, Qt::red);
			colSatLbl->setPalette(colPal);
		}
		break;
		case SATURATED_LOW:
		{
			colPal.setColor(QPalette::Window, Qt::black);
			colSatLbl->setPalette(colPal);
		}
		break;
		case NOT_SATURATED:
		{
			colPal.setColor(QPalette::Window, Qt::green);
			colSatLbl->setPalette(colPal);
		}
		break;
		default:
		{
			AHNS_ERROR("GroundStationForm::set_output_bars: invalid saturation state.");
		}
	}

	QPalette rudPal(rudSatLbl->palette());
	switch (rudSat)
	{
		case SATURATED_HIGH:
		{
			rudPal.setColor(QPalette::Window, Qt::red);
			rudSatLbl->setPalette(rudPal);
		}
		break;
		case SATURATED_LOW:
		{
			rudPal.setColor(QPalette::Window, Qt::black);
			rudSatLbl->setPalette(rudPal);
		}
		break;
		case NOT_SATURATED:
		{
			rudPal.setColor(QPalette::Window, Qt::green);
			rudSatLbl->setPalette(rudPal);
		}
		break;
		default:
		{
			AHNS_ERROR("GroundStationForm::set_output_bars: invalid saturation state.");
		}
	}

	if(latPID[0] > lateralPBar->maximum())
		latPID[0] = lateralPBar->maximum();
	if(latPID[0] < lateralPBar->minimum())
		latPID[0] = lateralPBar->minimum();
	lateralPBar->setValue(latPID[0]);
	if(latPID[1] > lateralIBar->maximum())
		latPID[1] = lateralIBar->maximum();
	if(latPID[1] < lateralIBar->minimum())
		latPID[1] = lateralIBar->minimum();
	lateralIBar->setValue(latPID[1]);
	if(latPID[2] > lateralDBar->maximum())
		latPID[2] = lateralDBar->maximum();
	if(latPID[2] < lateralDBar->minimum())
		latPID[2] = lateralDBar->minimum();
	lateralDBar->setValue(latPID[2]);

	if(lonPID[0] > longitudinalPBar->maximum())
		lonPID[0] = longitudinalPBar->maximum();
	if(lonPID[0] < longitudinalPBar->minimum())
		lonPID[0] = longitudinalPBar->minimum();
	longitudinalPBar->setValue(lonPID[0]);
	if(lonPID[1] > longitudinalIBar->maximum())
		lonPID[1] = longitudinalIBar->maximum();
	if(lonPID[1] < longitudinalIBar->minimum())
		lonPID[1] = longitudinalIBar->minimum();
	longitudinalIBar->setValue(lonPID[1]);
	if(lonPID[2] > longitudinalDBar->maximum())
		lonPID[2] = longitudinalDBar->maximum();
	if(lonPID[2] < longitudinalDBar->minimum())
		lonPID[2] = longitudinalDBar->minimum();
	longitudinalDBar->setValue(lonPID[2]);

	if(colPID[0] > collectivePBar->maximum())
		colPID[0] = collectivePBar->maximum();
	if(colPID[0] < collectivePBar->minimum())
		colPID[0] = collectivePBar->minimum();
	collectivePBar->setValue(colPID[0]);
	if(colPID[1] > collectiveIBar->maximum())
		colPID[1] = collectiveIBar->maximum();
	if(colPID[1] < collectiveIBar->minimum())
		colPID[1] = collectiveIBar->minimum();
	collectiveIBar->setValue(colPID[1]);
	if(colPID[2] > collectiveDBar->maximum())
		colPID[2] = collectiveDBar->maximum();
	if(colPID[2] < collectiveDBar->minimum())
		colPID[2] = collectiveDBar->minimum();
	collectiveDBar->setValue(colPID[2]);

	if(rudPID[0] > rudderPBar->maximum())
		rudPID[0] = rudderPBar->maximum();
	if(rudPID[0] < rudderPBar->minimum())
		rudPID[0] = rudderPBar->minimum();
	rudderPBar->setValue(rudPID[0]);
	if(rudPID[1] > rudderIBar->maximum())
		rudPID[1] = rudderIBar->maximum();
	if(rudPID[1] < rudderIBar->minimum())
		rudPID[1] = rudderIBar->minimum();
	rudderIBar->setValue(rudPID[1]);
	if(rudPID[2] > rudderDBar->maximum())
		rudPID[2] = rudderDBar->maximum();
	if(rudPID[2] < rudderDBar->minimum())
		rudPID[2] = rudderDBar->minimum();
	rudderDBar->setValue(rudPID[2]);

	// Log the data
	log_data();

	if(temp_cmd.lateral > lateralBar->maximum())
		temp_cmd.lateral = lateralBar->maximum();
	if(temp_cmd.lateral < lateralBar->minimum())
		temp_cmd.lateral = lateralBar->minimum();
	lateralBar->setValue(temp_cmd.lateral);
	if(temp_cmd.longitudinal > longitudinalBar->maximum())
		temp_cmd.longitudinal = longitudinalBar->maximum();
	if(temp_cmd.longitudinal < longitudinalBar->minimum())
		temp_cmd.longitudinal = longitudinalBar->minimum();
	longitudinalBar->setValue(temp_cmd.longitudinal);
	if(temp_cmd.collective > collectiveBar->maximum())
		temp_cmd.collective = collectiveBar->maximum();
	if(temp_cmd.collective < collectiveBar->minimum())
		temp_cmd.collective = collectiveBar->minimum();
	collectiveBar->setValue(temp_cmd.collective);
	if(temp_cmd.rudder > rudderBar->maximum())
		temp_cmd.rudder = rudderBar->maximum();
	if(temp_cmd.rudder < rudderBar->minimum())
		temp_cmd.rudder = rudderBar->minimum();
	rudderBar->setValue(temp_cmd.rudder);
//	sys_rate->start();
}

/**
 * Sends the commands as set by the sliders to the helicopter. This is direct manual control, these values go straight to the control surfaces.
 */
void GroundStationForm::slider_send()
{
	AHNS_DEBUG("GroundStationForm::slider_send");
	
	if (ap_mode == manual)
	{
		if(!debugChk->isChecked())
		{	
			heli_command command;
	
			command.lateral = lateralSlider->value();
			command.longitudinal = longitudinalSlider->value();
			command.collective = collectiveSlider->value();
			command.rudder = rudderSlider->value();

		//	AHNS_DEBUG("slider_send:: Sending manual command");
			transmit(command, emily_trim, groundpulse_port);
		}
		else
		{
			heli_command_db command;

			command.channel[0] = ch1Sld->value();
			command.channel[1] = ch2Sld->value();
			command.channel[2] = ch3Sld->value();
			command.channel[3] = ch4Sld->value();
			command.channel[4] = ch5Sld->value();
			command.channel[5] = ch6Sld->value();
			command.channel[6] = ch7Sld->value();

			transmit_db(command, groundpulse_port);
		}	
	}
}

/**
 * Increments the lost sync counter display.
 */
void GroundStationForm::lost_sync_counter()
{
	lostSyncCountLcd->display(lostSyncCountLcd->intValue()+1);
}

/**
 * Increments the discarded packet counter display.
 */
void GroundStationForm::discarded_packet_counter()
{
	discardedPacketCountLcd->display(discardedPacketCountLcd->intValue()+1);
}

/**
 * Updates the status bar on the GUI.
 */
void GroundStationForm::updateStatus()
{
	AHNS_DEBUG("GroundStationForm::updateStatus()");
	char theMsg[100];

	if(theStatus.printStatus(theMsg, 100) == 0)
	{
		AHNS_DEBUG("updateStatus: the message: ( " << theMsg << " )");
		statusbar->showMessage(tr(theMsg));
	}
}

/**
 * Initialises the data log by opening the file and writing the column headers.
 */
void GroundStationForm::initialise_datalog()
{
	AHNS_DEBUG("GroundStationForm::initialise_datalog()");
	datalog.open("datalog.ahns",ios::out);
	AHNS_DEBUG("GroundStationForm::initialise_datalog:: file opened");
	datalog << "year, month, day, hour, minute, second, millisecond, "
		<< "altitude_command, x_command, y_command, "
		<< "roll_command, pitch_command, yaw_command, "
		<< "altitude, roll, pitch, yaw, "
		<< "x_pos, y_pos, telem_time, "
		<< "alt_p, alt_i, alt_d, "
		<< "collective_cmd, "
		<< "roll_p, roll_i, roll_d, "
		<< "pitch_p, pitch_i, pitch_d, "
		<< "yaw_p, yaw_i, yaw_d, " 
		<< "alt_p_gain, alt_i_gain, alt_d_gain, "
		<< "roll_p_gain, roll_i_gain, roll_d_gain, "
		<< "pitch_p_gain, pitch_i_gain, pitch_d_gain, "
		<< "yaw_p_gain, yaw_i_gain, yaw_d_gain, " 
		<< "battery" << endl;
	AHNS_DEBUG("GroundStationForm::initialise_datalog:: COMPLETE");
}

/**
 * Writes one new line to the data logger. 
 */
void GroundStationForm::log_data()
{
	AHNS_DEBUG("GroundStationForm::log_data()");
	char year[5];
	char month[3];
	char day[3];
	QTime theQtime;

	time_t rawtime;
	tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	theQtime = QTime::currentTime();

	strftime(year,5,"%Y",timeinfo);
	strftime(month,3,"%m",timeinfo);
	strftime(day,3,"%d",timeinfo);

	AHNS_DEBUG("GroundStationForm::log_data:: writing to file");
	datalog << year << ", " << month << ", " << day << ", " << theQtime.hour() << ", " << theQtime.minute() << ", " <<  theQtime.second() << ", " << theQtime.msec() << ", "
		<< logdata.alt_command << ", " << logdata.x_command << ", " << logdata.y_command << ", " 
		<< logdata.roll_command << ", " << logdata.pitch_command << ", " << logdata.yaw_command << ", " 
		<< logdata.altitude << ", " << logdata.roll << ", " << logdata.pitch << ", " << logdata.yaw << ", "
		<< logdata.x_pos << ", " << logdata.y_pos << ", " << logdata.telem_time << ", " 
		<< logdata.alt_p << ", " << logdata.alt_i << ", " << logdata.alt_d << ", "
		<< logdata.collective_cmd << ", "
		<< logdata.roll_p << ", " << logdata.roll_i << ", " << logdata.roll_d << ", "
		<< logdata.pitch_p << ", " << logdata.pitch_i << ", " << logdata.pitch_d << ", "
		<< logdata.yaw_p << ", " << logdata.yaw_i << ", " << logdata.yaw_d << ", " 
		<< logdata.alt_p_gain << ", " << logdata.alt_i_gain << ", " << logdata.alt_d_gain << ", "
		<< logdata.roll_p_gain << ", " << logdata.roll_i_gain << ", " << logdata.roll_d_gain << ", "
		<< logdata.pitch_p_gain << ", " << logdata.pitch_i_gain << ", " << logdata.pitch_d_gain << ", " 
		<< logdata.yaw_p_gain << ", " << logdata.yaw_i_gain << ", " << logdata.yaw_d_gain << ", "
		<< logdata.battery << endl;
	AHNS_DEBUG("GroundStationForm::log_data:: COMPLETE");
}

/**
 * Initialise the UDP receiving port for receiving HMI data.
 */
void GroundStationForm::hmi_rx_initialise()
{
	hmi_rx_sock = new QUdpSocket;
	hmi_rx_sock->bind(HMI_RX_PORT);
}

/**
 * Receive data through the UDP port.
 */
void GroundStationForm::hmi_receive()
{
	AHNS_DEBUG("GroundStationForm::hmi_receive()")
	while(hmi_rx_sock->hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(hmi_rx_sock->pendingDatagramSize());
		hmi_rx_sock->readDatagram(datagram.data(), datagram.size());
		char *byte = new char;
		byte = datagram.data();
		hmi_rx_machine((uint8_t*)byte);
	}
	AHNS_DEBUG("GroundStationForm::hmi_receive: COMPLETE");
}

/**
 * The state machine which deals with one byte at a time from the UDP port receiving HMI data.
 */
void GroundStationForm::hmi_rx_machine(uint8_t *byte)
{
	using namespace hmi_packet;
	static hmi_state state = S1;
	static char buffer[HMI_PACKSIZE];
	static int buffer_index = 0;
	

	switch(state)
	{
		case S1:
			if(*byte == HMI_SYNC_1)
			{
				state = S2;
			}
			else
			{
				state = S1;
			}
		break;
		case S2:
			if(*byte == HMI_SYNC_2)
			{
				state = S3;
			}
			else if(*byte == HMI_SYNC_1)
			{
				state = S2;
			}
			else
			{
				state = S1;
			}
		break;
		case S3:
			if(*byte == HMI_SYNC_3)
			{
				state = S4;
			}
			else
			{
				state = S1;
			}
		break;
		case S4:
			if(*byte == HMI_SYNC_4)
			{
				state = S5;
			}
			else
			{
				state = S1;
			}
		break;
		case S5:
			buffer[buffer_index] = *byte;
			buffer_index++;
		
			if(buffer_index < HMI_PACKSIZE)
			{
				state = S5;
			}
			else
			{
				state = S6;
				buffer_index = 0;
			}
		break;
		case S6:
			if(*byte == HMI_SYNC_1)
			{
				state = S7;
			}
			else
			{
				state = S1;
			}
		break;
		case S7:
			if(*byte == HMI_SYNC_2)
			{
				state = S8;
			}
			else
			{
				state = S1;
			}
		break;
		case S8:
			if(*byte == HMI_SYNC_3)
			{
				state = S9;
			}
			else
			{
				state = S1;
			}
		break;
		case S9:
			if(*byte == HMI_SYNC_4)
			{
				state = S5;
				// packet was good - store it				
				hmi_packet.timestamp  	= (buffer[0]<<24) + (buffer[1]<<16) + (buffer[2]<<8) + buffer[3];
				hmi_packet.joystick_x 	= (buffer[4]<<8) + (buffer[5]);
				hmi_packet.joystick_y 	= (buffer[6]<<8) + (buffer[7]);
				hmi_packet.joystick_z 	= (buffer[8]<<8) + (buffer[9]);
				hmi_packet.ap_mode    	= buffer[10];
				hmi_packet.ap_cmd_x   	= buffer[11];
				hmi_packet.ap_cmd_y	= buffer[12];
				hmi_packet.ap_cmd_alt	= (buffer[13]<<8) + (buffer[14]);
				hmi_display();
			}
			else
			{
				state = S1;
			}
		break;
	}

}

/**
 * Display the data received from the HMI computer
 */
void GroundStationForm::hmi_display()
{
	char time[20];

	if(hmi_packet.ap_mode == AUTO)
	{
		hmiLbl->setText("AUTO");
	}
	else if(hmi_packet.ap_mode == STAB)
	{
		hmiLbl->setText("STAB AUG");
	}
	else
	{
		hmiLbl->setText("ERROR");
	}

	hmiJoyXBar->setValue(hmi_packet.joystick_x);
        hmiJoyYBar->setValue(hmi_packet.joystick_y);
        hmiJoyZBar->setValue(hmi_packet.joystick_z);

        hmiCmdXBar->setValue(hmi_packet.ap_cmd_x);
        hmiCmdYBar->setValue(hmi_packet.ap_cmd_y);
        hmiCmdZBar->setValue(hmi_packet.ap_cmd_alt);

	int hours = (int)(hmi_packet.timestamp/3600000.0);
	int minutes = (int)((hmi_packet.timestamp - hours*3600000)/60000.0);
	int seconds = (int)((hmi_packet.timestamp - hours*3600000 - minutes*60000)/1000.0);
	int hundredths = (int)((hmi_packet.timestamp - hours*3600000 - minutes*60000 - seconds*1000)/10);
	
	sprintf(time,"%02i:%02i . %02i%c",minutes,seconds,hundredths,'\0');

	hmiTimeLcd->display(time);

}

/**
 * Initialise the UDP connection to the HMI 
 */
void GroundStationForm::hmi_tx_initialise()
{
	AHNS_DEBUG("GroundStationForm::hmi_tx_initialise()")
	hmi_tx_sock = new QUdpSocket();
	if (hmi_tx_sock == NULL)
	{
		AHNS_ALERT("GroundStationForm::hmi_tx_initialise: could not open UDP socket!");
	}
	hmi_tx_sock->connectToHost(HMI_IP, HMI_TX_PORT);
	AHNS_DEBUG("GroundStationForm::hmi_tx_initialise: COMPLETED");
}

/**
 * Send a data packet to the HMI
 */
void GroundStationForm::hmi_send()
{
	AHNS_DEBUG("GroundStationForm::hmi_send()");
	int time;
	int ii;
	uint8_t buffer[AP_BUFFSIZE];
	QTime theQtime;
	theQtime = QTime::currentTime();
	time = theQtime.hour() * 3.6e6 + theQtime.minute() * 6.0e4 + theQtime.second() * 1e3 + theQtime.msec();

	AHNS_DEBUG("GroundStationForm::hmi_send: filling buffer")

	buffer[0] = AP_SYNC_1;
	buffer[1] = AP_SYNC_2;
	buffer[2] = AP_SYNC_3;
	buffer[3] = AP_SYNC_4;

	buffer[4] = (time&0xFF000000)>>(3*8);
	buffer[5] = (time&0x00FF0000)>>(2*8);
	buffer[6] = (time&0x0000FF00)>>(1*8);
	buffer[7] = (time&0x000000FF)>>(0*8);
	
	int alt_cmd = logdata.alt_command*1000; // altitude command in metres, send in millimetres 
	buffer[8] = (alt_cmd&0xFF000000)>>(3*8);
	buffer[9] = (alt_cmd&0x00FF0000)>>(2*8);
	buffer[10] = (alt_cmd&0x0000FF00)>>(1*8);
	buffer[11] = (alt_cmd&0x000000FF)>>(0*8);

	int alt = logdata.altitude*10;	// altitude command in centimetres, send in millimetres
	buffer[12]  = (alt&0xFF000000)>>(3*8);
	buffer[13]  = (alt&0x00FF0000)>>(2*8);
	buffer[14] = (alt&0x0000FF00)>>(1*8);
	buffer[15] = (alt&0x000000FF)>>(0*8);

	int alt_p = logdata.alt_p*100;
	buffer[16] = (alt_p&0xFF000000)>>(3*8);
	buffer[17] = (alt_p&0x00FF0000)>>(2*8);
	buffer[18] = (alt_p&0x0000FF00)>>(1*8);
	buffer[19] = (alt_p&0x000000FF)>>(0*8);

	int alt_i = logdata.alt_i*100;
	buffer[20] = (alt_i&0xFF000000)>>(3*8);
	buffer[21] = (alt_i&0x00FF0000)>>(2*8);
	buffer[22] = (alt_i&0x0000FF00)>>(1*8);
	buffer[23] = (alt_i&0x000000FF)>>(0*8);

	int alt_d = logdata.alt_d*100;
	buffer[24] = (alt_d&0xFF000000)>>(3*8);
	buffer[25] = (alt_d&0x00FF0000)>>(2*8);
	buffer[26] = (alt_d&0x0000FF00)>>(1*8);
	buffer[27] = (alt_d&0x000000FF)>>(0*8);	

	int collective = logdata.collective_cmd*100;
	buffer[28] = (collective&0xFF000000)>>(3*8);
	buffer[29] = (collective&0x00FF0000)>>(2*8);
	buffer[30] = (collective&0x0000FF00)>>(1*8);
	buffer[31] = (collective&0x000000FF)>>(0*8);

	uint16_t batt = logdata.battery;
	buffer[32] = 0;
	buffer[33] = 0x00FF&batt;

	AHNS_DEBUG("GroundStationForm::hmi_send: sending buffer");
	for (ii=0; ii<AP_BUFFSIZE; ii++)
	{
		AHNS_DEBUG("GroundStationForm::hmi_send: sending one byte.");
		hmi_tx_sock->write((char*)(buffer+ii), 1);
	}
	AHNS_DEBUG("GroundStationForm::hmi_send: COMPLETED.");
}

/**
 * Closes the data logging file.
 */
void GroundStationForm::close_datalog()
{
	AHNS_DEBUG("GroundStationForm::close_datalog()")
	datalog.close();
}

/**
 * Shut down the application.
 */
void GroundStationForm::closeEvent(QCloseEvent* event)
{
	AHNS_DEBUG("GroundStationForm::on_closeBtn_clicked()");
	AHNS_DEBUG("on_closeBtn_clicked: attempting to close serial port... ");
	::close(telemetry_port);
	::close(groundpulse_port);
	AHNS_DEBUG("on_closeBtn_clicked: serial port closed");

	close_datalog();

	slider_timer->stop();
	hmi_send_timer->stop();

	if(hmi_rx_sock != NULL)
	{
		delete hmi_rx_sock;
	}
	if(hmi_tx_sock != NULL)
	{
		delete hmi_tx_sock;
	}

	if(telemetry_thread != NULL)
	{
		telemetry_thread->stop();
		AHNS_DEBUG("on_closeBtn_clicked: telemetry thread asked to stop");	
		telemetry_thread->wait();
		AHNS_DEBUG("on_closeBtn_clicked: telemetry thread stopped");
		delete telemetry_thread;
		telemetry_thread = NULL;
		AHNS_DEBUG("on_closeBtn_clicked: telemetry thread deleted");
	}
	else
	{
		AHNS_DEBUG("on_closeBtn_clicked: telemetry_thread was NULL.");
	}

	if(emily_telem != NULL)
	{
		AHNS_DEBUG("on_closeBtn_clicked: deleting telemetry structure...");
		delete emily_telem;
		emily_telem = NULL;
		AHNS_DEBUG("on_closeBtn_clicked:\t\tdone.");
	}

	if(emily_ap != NULL)
	{
		AHNS_DEBUG("on_closeBtn_clicked: wating for autopilot thread to stop...");
		emily_ap->wait();
		AHNS_DEBUG("on_closeBtn_clicked: stopped. Deleting autopilot thread...");
		delete emily_ap;
		emily_ap = NULL;
		AHNS_DEBUG("on_closeBtn_clicked: deleted.");
	}

	if(gains.is_open())
	{
		AHNS_DEBUG("on_closeBtn_clicked: closing gains file...");
		gains.close();
		AHNS_DEBUG("on_closeBtn_clicked:\t\tclosed.");
	}
	if(trims.is_open())
	{
		AHNS_DEBUG("on_closeBtn_clicked: closing trims file...");
		trims.close();
		AHNS_DEBUG("on_closeBtn_clicked:\t\tclosed.");
	}

	if(hmi_send_timer != NULL)
		delete hmi_send_timer;

	AHNS_DEBUG("on_closeBtn_clicked: deleting sys_rate...");
	if(sys_rate != NULL)
		delete sys_rate;
	AHNS_DEBUG("on_closeBtn_clicked:\t\tdone.");
	AHNS_DEBUG("on_closeBtn_clicked: deleting slider_timer...");
	if(slider_timer != NULL)
		delete slider_timer;
	AHNS_DEBUG("on_closeBtn_clicked\t\tdone.");
	AHNS_DEBUG("on_closeBtn_clicked:\t\tdeleted.");

	AHNS_DEBUG("on_closeBtn_clicked: \t[ COMPLETED SUCCESSFULLY ].");
}

/**
 * Slot from exit button. Shuts down the application by calling close. 
 */
void GroundStationForm::on_closeBtn_clicked()
{
	close();
}

//////////////////////////

/**
 * Constructor for the ahnsStatus class.
 */
ahnsStatus::ahnsStatus()
{
	telemetry = -1;
	groundpulse = -1;
	hmi = -1;
}

/**
 * Change the telemetry status on the status bar. 
 * @param  status The status.
 */
void ahnsStatus::setTelemetry(int status)
{
	telemetry = status;
}

/**
 * Change the groundpulse status on the status bar. 
 * @param  status The status.
 */
void ahnsStatus::setGroundpulse(int status)
{
	groundpulse = status;
}

/**
 * Change the HMI status on the status bar.
 * @param  status The status.
 */
void ahnsStatus::setHMI(int status)
{
	hmi = status;
}

/**
 * Print to a buffer the status to display in the status bar.
 * @param *buff   The buffer in which the string is written.
 * @param buffMax The size of the buffer.
 */
int  ahnsStatus::printStatus(char* buff, int buffMax)
{
	char *szTelemetry = new char[16];
	char *szGroundpulse = new char[16];
	char *szHmi = new char[16];
	int n, n_telem, n_groundpulse, n_hmi;

	switch(telemetry)
	{
		case OK:
			n_telem = sprintf(szTelemetry, "OK");
		break;
		case NO_PORT:
			n_telem = sprintf(szTelemetry, "No Port");
		break;
		default:
			n_telem = sprintf(szTelemetry, "Unknown Error");
		break;
	}
	switch(groundpulse)
	{
		case OK:
			n_groundpulse = sprintf(szGroundpulse, "OK");
		break;
		case NO_PORT:
			n_groundpulse = sprintf(szGroundpulse, "No Port");
		break;
		default:
			n_groundpulse = sprintf(szGroundpulse, "Unknown Error");
		break;
	}
	switch(hmi)
	{
		case OK:
			n_hmi = sprintf(szHmi, "OK");
		break;
		case NO_PORT:
			n_hmi = sprintf(szHmi, "No Port");
		break;
		default:
			n_hmi = sprintf(szHmi, "Unknown Error");
		break;
	}	
	
	n = sprintf(buff, "STATUS:   Telemetry: %s   |   Groundpulse: %s   |   HMI: %s   |", szTelemetry, szGroundpulse, szHmi);

	delete [] szTelemetry;
	delete [] szGroundpulse;
	delete [] szHmi;
	
	return 0;

}
