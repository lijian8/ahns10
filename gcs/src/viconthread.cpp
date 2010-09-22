/**
 * @file   telemetrythread.cpp
 * @author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-08-14 14:58:08 +1000 (Sat, 14 Aug 2010) $
 * $Rev: 317 $
 * $Id: telemetrythread.cpp 317 2010-08-14 04:58:08Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * TelemetryThread class implementation
 */

#include <QThread>
#include <QMutexLocker>
#include <QtNetwork>
#include <QMessageBox>

#include "viconthread.h"
#include "ahns_logger.h"
#include "ahns_timeformat.h"

#include <sys/time.h>
#include <sys/socket.h>
#include "inttypes.h"
#include <netinet/in.h>
#include <stdexcept>
#include <ctime>

#include "state.h"
#include "Client.h"

using namespace ViconDataStreamSDK::CPP;

/** Vicon client */
Client myClient;
/** Vicon unit conversions */
#define mm2M                0.01

/**
  * @brief Default constructor
  * @param parent The object to be set as the parent of the thread, default 0
  */
ViconThread::ViconThread(QObject * parent)
    : QThread(parent)
{
    AHNS_DEBUG("ViconThread::ViconThread(QObject)");

    moveToThread(this);
    start();
}

/**
  * @brief Constructor to start a new UDP Telemetry Thread from ints
  * @param serverPort The port of the server that the connection will be to
  * @param serverIP The IP of the server
  * @param parent The object to be set as the parent of the thread, default 0
  */
ViconThread::ViconThread(quint16& serverPort, QString& serverIP, QObject * parent)
    : m_serverPort(serverPort), m_serverIP(serverIP), m_connected(true), m_stopped(false), QThread(parent)
{
    AHNS_DEBUG("ViconThread::ViconThread(server,client)");
    QMutexLocker lock(&m_mutex);

    moveToThread(this);
    start();
}

/**
  * @brief Parameter Copy Constructor
  * @param serverPort Port to connect to on the server
  * @param serverIP QHostAddress of the server
  * @param parent The object to be set as the parent of the thread, default 0
  */
ViconThread::ViconThread(quint16& serverPort, QHostAddress& serverIP, QObject * parent)
    : m_serverPort(serverPort), m_serverIP(serverIP), m_connected(true), m_stopped(false), QThread(parent)
{
    AHNS_DEBUG("ViconThread::ViconThread(server,client QHostAddress)");
    QMutexLocker lock(&m_mutex);

    moveToThread(this);
    start();
}

/**
  * @brief Destructor stops the thread
  * Sockets are children of the thread so they will be closed and destroyed automatically
  */
ViconThread::~ViconThread()
{
    AHNS_DEBUG("ViconThread::~ViconThread()");
    QMutexLocker locker(&m_mutex);
}

int ViconThread::ViconServerConnect()
{
    int retValue = 0;

    std::string Hostname;
    Hostname = m_serverIP.toString().toStdString()+":"+ QString("%1").arg(m_serverPort).toStdString();

    //std::cout << "Trying to connect" << std::endl;
    myClient.Connect(Hostname);
    if (myClient.IsConnected().Connected)
    {
        // connection success, enable data types
        myClient.EnableSegmentData();
        myClient.EnableMarkerData();
        myClient.EnableUnlabeledMarkerData();
        myClient.EnableDeviceData();
        // set the streaming mode from the server
        myClient.SetStreamMode(ViconDataStreamSDK::CPP::StreamMode::ClientPull);
        // set the global axis
        myClient.SetAxisMapping(Direction::Forward, Direction::Left, Direction::Up);
        retValue = 1;
        //emit ViconProcessReady();
    }
    return retValue;
}

void ViconThread::ProcessViconState()
{
    // position of the vicon object
    double X,Y,Z;
    // orientation of the vicon object
    double phi,theta,psi;
    // velocity of the vicon object
    double vx,vy,vz;

    std::string SegmentName;
    std::string SubjectName;
    unsigned int SubjectCount;
    unsigned int SegmentCount;
    while(myClient.GetFrame().Result != Result::Success)
      {
        sleep(1);
        std::cout << ".";
      }
    //number of objects currently represented/tracked by the vicon system
    SubjectCount = myClient.GetSubjectCount().SubjectCount;
    unsigned int SubjectIndex = 0;
    SubjectName = myClient.GetSubjectName(SubjectIndex).SubjectName;
    // Get the root segment
    //std::string RootSegment = myClient.GetSubjectRootSegmentName( SubjectName ).SegmentName;
    //Get the number of Segments
    SegmentCount = myClient.GetSegmentCount(SubjectName).SegmentCount;
    unsigned int SegmentIndex = 0;
    //Get the name of Segment
    SegmentName = myClient.GetSegmentName(SubjectName, SegmentIndex).SegmentName;
    //Get the name of subject
    SubjectName = myClient.GetSubjectName(SubjectIndex).SubjectName;

    // Get the global segment translation
    Output_GetSegmentGlobalTranslation _Output_GetSegmentGlobalTranslation = myClient.GetSegmentGlobalTranslation(SubjectName, SegmentName);

    //store locally X,Y,Z. data comes by defauls in mm and radians
    X = _Output_GetSegmentGlobalTranslation.Translation[ 0 ]*mm2M;
    Y = _Output_GetSegmentGlobalTranslation.Translation[ 1 ]*mm2M;
    Z = -1.0 * (_Output_GetSegmentGlobalTranslation.Translation[ 2 ])*mm2M;

    // Get the global segment orientation
    Output_GetSegmentGlobalRotationEulerXYZ _Output_GetSegmentGlobalRotationEulerXYZ = myClient.GetSegmentGlobalRotationEulerXYZ(SubjectName,SegmentName);

    //store locally roll, pitch, yaw
    phi = _Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 0 ];
    theta = _Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 1 ];
    psi =  _Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 2 ];

    //print in console current position and attitude
    std::cout << "Global Translation: (" << X << ", " << Y << ", " << Z << ") " << std::endl;
    std::cout << "Global Orientation(" << phi << ", " << theta << ", " << psi << ") " << std::endl;

    // allocate vicon data to vicon_state_t
    vicon_state_t viconState;
    viconState.phi = phi;
    viconState.theta = theta;
    viconState.psi = psi;
    viconState.x = X;
    viconState.y = Y;
    viconState.z = Z;
    viconState.vx = vx;
    viconState.vy = vy;
    viconState.vz = vz;

    emit NewViconState(viconState);

    // all vicon data received, get the next frame
}

/**
  * @brief Loops the telemetry thread routine
  *
  * Begins by attempting to connect to the server with  a 25 sec timeout.
  */
void ViconThread::run()
{
    AHNS_DEBUG("ViconThread::run() [ STARTED ]");
    AHNS_DEBUG("ViconThread::run() [ VICON THREAD ID " << (int) QThread::currentThreadId() << " ]");

    if (ViconServerConnect())
    {
        m_connected = true;
        while (!m_stopped)
        {
            std::cerr << "Vicon Thread running.." <<endl;
            //ProcessViconState();
            msleep(5);
            //exec();
        }
        myClient.Disconnect();
        m_connected = false;
    }
    else
    {
        m_connected = false;
    }

    AHNS_DEBUG("ViconThread::run() [ COMPLETED ] ");
    return;
}

/**
  * @brief Method to call to stop the thread
  *
  * This needs to be called from the ViconThread thread to avoid blocking the GUI
  */
void ViconThread::stop()
{
    AHNS_DEBUG("ViconThread::stop()");

    QMutexLocker locker (&m_mutex);
    m_stopped = true;
    //quit();

    return;
}

/**
  * @brief Accessort function for m_serverPort
  */
quint16 ViconThread::readServerPort() const
{
    return m_serverPort;
}

/**
  * @brief Accessor function for m_serverIP
  */
QHostAddress ViconThread::readServerIP() const
{
    return m_serverIP;
}

/**
  * @brief Accessor function for m_connect
  */
bool ViconThread::isConnected() const
{
    return m_connected;
}
