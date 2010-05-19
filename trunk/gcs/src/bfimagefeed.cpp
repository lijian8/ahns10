/**
 * @file   bfimagefeed.cpp
 * @author Liam O'Sullivan
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Blackfin camera feed widget which:
 * - Creates a tcp connection to the blackfin camera (bfcameracomms)
 * - Creates a opengl render widget to display the image feed
 *
 */

#include "bfcameracomms.h"
#include "bfglrenderer.h"
#include "bfimagefeed.h"

/**
  * @brief Constructor
  */
bfImageFeed::bfImageFeed(QWidget *parent) : QWidget(parent)
{

    glWidget = new bfglrenderer;
    bfCam = new bfcameracomms;
    glWidget->connectIOhandler(bfCam);
    state = 1;

    connectButton = new QPushButton("Connect");
    connectButton->setFixedWidth(100);
    serverLineEdit = new QLineEdit();
    serverLineEdit->setText(DEFAULT_CAM_IP);
    fpsText = new QLabel("0.0");

    // signals and slots
    connect(connectButton, SIGNAL(clicked()), this, SLOT(setconnectButtonState()));
    connect(this, SIGNAL(buttonConnect()), this, SLOT(connectServer()));
    connect(this, SIGNAL(buttonDisconnect()), bfCam, SLOT(server_disconnect()));
    connect(bfCam,SIGNAL(SIG_imageWritten()),this,SLOT(updateLabel()));

    QGridLayout *subLayout = new QGridLayout;
    subLayout->addWidget(connectButton, 0, 0);
    subLayout->addWidget(serverLineEdit, 0 ,1);
    subLayout->addWidget(fpsText, 0, 2);
    subLayout->setColumnMinimumWidth(1,160);
    subLayout->setColumnMinimumWidth(2,40);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(subLayout);
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);

    setWindowTitle(tr("Blackfin Camera Feed"));
}

/**
  * @brief Destructor
  */
void bfImageFeed::setconnectButtonState()
{
    if (state)
    {
        connectButton->setText("Disconnect");
        emit buttonConnect();
    } else
        {
        emit buttonDisconnect();
        connectButton->setText("Connect");
    }
    state = !state;

}

/**
  * @brief Send the server line edit data to server connect
  */
void bfImageFeed::connectServer()
{
    bfCam->server_connect(serverLineEdit->text());
}

/**
  * @brief Update the fps label from camera comms
  */
void bfImageFeed::updateLabel()
{
    fpsText->setText(QString::number((bfCam->fps),'g',4));
}
