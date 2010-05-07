/**
 * @file   wificomms.cpp
 * @author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Wifi Communications widget to:
 *   - Receive the desired server IP/hostname
 *   - Requies Launch the wifi telemetry thread
 */

#include "include/wificomms.h"
#include "ui_wificomms.h"

#include <QString>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <iomanip>
#include <cstring>
#include <unistd.h>

#include <errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include "ahns_logger.h"
#include "ahns_timeformat.h"

// Default Network Settings
#define DEFAULT_SERVER_IP "192.168.0.3"
#define DEFAULT_SERVER_PORT "2002"
#define DEFAULT_CLIENT_PORT "45455"


wifiComms::wifiComms(QWidget *parent) : QWidget(parent), ui(new Ui::wifiComms)
{

    ui->setupUi(this);

    // 2009 HMI code to find the interface and IP in use
    using namespace std;
    int fd;
    struct if_nameindex *curif, *ifs;
    struct ifreq req;

    if((fd = socket(PF_INET, SOCK_DGRAM, 0)) != -1)
    {
            ifs = if_nameindex();
            if(ifs)
            {
                    for(curif = ifs; curif && curif->if_name; curif++)
                    {
                            strncpy(req.ifr_name, curif->if_name, IFNAMSIZ);
                            req.ifr_name[IFNAMSIZ] = 0;

                            if (ioctl(fd, SIOCGIFADDR, &req) >= 0)
                            {
                                    //printf("%s: [%s]\n", curif->if_name, inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr));
                                    if (string(curif->if_name) == "eth0")
                                    {
                                            sprintf(szHostIP, "%s", inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr));
                                    }
                                    else if (string(curif->if_name) == "wlan0")
                                    {
                                            sprintf(szHostIP, "%s", inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr));
                                            //cout << string(szHostIP) << endl;
                                    }
                                    else if (string(curif->if_name) == "eth1") //MBP interface
                                    {
                                            sprintf(szHostIP, "%s", inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr));
                                    }
                            }
                    }
                    if_freenameindex(ifs);
                    if(::close(fd) != 0)	sprintf(szHostIP, "%s", "Unable to close 'fd' <internal>");
            }
            else	sprintf(szHostIP, "%s", "Could not retrieve IP name index");
    }
    else	sprintf(szHostIP, "%s", "Could not detect network socket");

    // Initialise the Box text
    ui->serverIPlineEdit->setText(DEFAULT_SERVER_IP);
    ui->serverPortlineEdit->setText(DEFAULT_SERVER_PORT);

    ui->clientIPlineEdit->setText(szHostIP);
    ui->clientPortlineEdit->setText(DEFAULT_CLIENT_PORT);

    // internal signals
    connect(&m_oUptimer,SIGNAL(timeout()),this,SLOT(lcdUpdate()));

    //timer variables
    m_oUptimer.setInterval(1000);
    m_secCount = 0;
    m_minCount = 0;
    m_hourCount = 0;

    ui->uptimelcdNumber->display(AHNS_HMS(m_hourCount,m_minCount,m_secCount));
}

wifiComms::~wifiComms()
{
    m_oUptimer.stop();
    delete ui;
}

void wifiComms::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * @brief Initial sizing hint of the wifi widget
  */
QSize wifiComms::sizeHint() const
{
    return QSize(400, 165);
}


/**
  * @brief Slot to emit signals used by the mainwindow to start
  * @param Pointer to button from the dialogue box
  */
void wifiComms::buttonBoxChanged(QAbstractButton* btnAbstract)
{
    // Take strings and numbers from the dialogue boxes
    quint16 serverPort = ui->serverPortlineEdit->text().toUInt();
    QString serverIP = ui->serverIPlineEdit->text();
    quint16 clientPort = ui->clientPortlineEdit->text().toUInt();
    QString clientIP = ui->clientIPlineEdit->text();

    AHNS_DEBUG("wifiComms::buttonBoxChanged()");
    // Find the signal and emit it, also manage timer
    if (btnAbstract->text() == "&Close")
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ Close ]");
        emit ConnectionClose();
        m_oUptimer.stop();

        m_secCount = 0;
        m_minCount = 0;
        m_hourCount = 0;

        ui->uptimelcdNumber->display(AHNS_HMS(m_hourCount,m_minCount,m_secCount));
    }
    else if (btnAbstract->text() == "Open")
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ Open ]");
        emit ConnectionStart(serverPort, serverIP, clientPort, clientIP);
        m_oUptimer.start();
    }
    else if (btnAbstract->text() == "Retry")
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ Retry ]");
        emit ConnectionRetry(serverPort, serverIP, clientPort, clientIP);

        m_oUptimer.start();
        m_secCount = 0;
        m_minCount = 0;
        m_hourCount = 0;
        ui->uptimelcdNumber->display(AHNS_HMS(m_hourCount,m_minCount,m_secCount));
    }
    else if (btnAbstract->text() == "Restore Defaults")
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ Restore Defaults ]");

        ui->serverIPlineEdit->setText(DEFAULT_SERVER_IP);
        ui->serverPortlineEdit->setText(DEFAULT_SERVER_PORT);
        ui->clientIPlineEdit->setText(szHostIP);
        ui->clientPortlineEdit->setText(DEFAULT_CLIENT_PORT);
    }
    else // ui is wrong
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ UNDEFINED BUTTON ]");
    }
}

/**
  * Slot for the 1 second Timer to update the Telemetry LCD
  */
void wifiComms::lcdUpdate()
{
    m_secCount++;
    if (m_secCount == 60)
    {
        m_secCount = 0;
        m_minCount++;
        if (m_minCount > 60)
        {
            m_minCount = 0;
            m_hourCount++;
        }
    }
    // Output to LCD
    ui->uptimelcdNumber->display(AHNS_HMS(m_hourCount,m_minCount,m_secCount));
    return;
}