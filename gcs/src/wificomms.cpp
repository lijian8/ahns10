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


wifiComms::wifiComms(QWidget *parent) : QWidget(parent), ui(new Ui::wifiComms)
{

    ui->setupUi(this);


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

    ui->serverIPlineEdit->setText("192.168.2.2");
    ui->serverPortlineEdit->setText("5000");

    ui->clientIPlineEdit->setText(szHostIP);
    ui->clientPortlineEdit->setText("5000");


    connect(&m_oUptimer,SIGNAL(timeout()),this,SLOT(lcdUpdate()));
    m_oUptimer.setInterval(1000);
    m_secCount = 0;
    m_minCount = 0;
    m_hourCount = 0;

    char time[20];
    sprintf(time,"%02i:%02i . %02i%c",0,0,0,'\0');
    ui->uptimelcdNumber->display(time);
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
    quint16 serverPort = ui->serverPortlineEdit->text().toUInt();
    QString serverIP = ui->serverIPlineEdit->text();

    quint16 clientPort = ui->clientPortlineEdit->text().toUInt();
    QString clientIP = ui->clientIPlineEdit->text();

    AHNS_DEBUG("wifiComms::buttonBoxChanged()");
    if (btnAbstract->text() == "&Close")
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ Close ]");
        emit sigConnectionClose();
        m_oUptimer.stop();

        m_secCount = 0;
        m_minCount = 0;
        m_hourCount = 0;
        char time[20];
        sprintf(time,"%02i:%02i . %02i%c",0,0,0,'\0');
        ui->uptimelcdNumber->display(time);
    }
    else if (btnAbstract->text() == "Apply")
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ Apply ]");
        emit sigConnectionStart(serverPort, serverIP, clientPort, clientIP);
        m_oUptimer.start();
    }
    else if (btnAbstract->text() == "Retry")
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ Retry ]");
        emit sigConnectionRetry(serverPort, serverIP, clientPort, clientIP);
        m_oUptimer.start();
        m_secCount = 0;
        m_minCount = 0;
        m_hourCount = 0;
        char time[20];
        sprintf(time,"%02i:%02i . %02i%c",0,0,0,'\0');
        ui->uptimelcdNumber->display(time);
    }
    else if (btnAbstract->text() == "Restore Defaults")
    {
        AHNS_DEBUG("wifiComms::buttonBoxChanged() [ Restore Defaults ]");
        ui->serverIPlineEdit->setText("192.168.2.2");
        ui->serverPortlineEdit->setText("5000");
        ui->clientIPlineEdit->setText(szHostIP);
        ui->clientPortlineEdit->setText("5000");
    }
    else
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
    char time[20];
    sprintf(time,"%02i:%02i . %02i%c",m_hourCount,m_minCount,m_secCount,'\0');
    ui->uptimelcdNumber->display(time);
    return;
}
