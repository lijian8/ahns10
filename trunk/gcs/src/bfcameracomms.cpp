/**
 * @file   bfcameracomms.cpp
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
 * Blackfin camera communication library:
 *   - Connect to blackfin camera over tcp
 *   - Request and receive images from the blackfin
 */

#include "bfcameracomms.h"

char IMAGE_HEADER[5] = {'#','#','I','M','J'};
int header_received = 0;
int frame_pixel_size_received = 0;
int frame_byte_size_received = 0;
int frame_byte_size = 0;
quint8 frame_pixel_size = 0;
int image_marker = 0;
int mark = 0;

/**
  * @brief Constructor
  */
bfcameracomms::bfcameracomms()
{
    // new tcp socket object
    theIODevice = &tcpSocket;
    timeTaken = 0;
    framesReceived = 0;
    fps = 0;
    t.start();
}

/**
  * @brief Destructor
  */
bfcameracomms::~bfcameracomms()
{

}

/**
  * @brief Connect to the blackfin camera
  */
void bfcameracomms::server_connect(QString server)
{
    std::cout << "connect" << std::endl;
    header_received = 0;
    frame_pixel_size_received = 0;
    frame_byte_size_received = 0;
    frame_byte_size = 0;
    frame_pixel_size = 0;
    image_marker = 0;
    mark = 0;
    tcpSocket.connectToHost(server,DEFAULT_CAM_PORT);
    tcpSocket.reset();
    image_buffer = new QDataStream(&tcpSocket);
    // signals and slots
    connect(&tcpSocket, SIGNAL(connected()), this, SLOT(sendImageRequest()));
    connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(getImage()));
    connect(this, SIGNAL(SIG_imageReceived()), this, SLOT(updateImage()));
    tcpSocket.waitForConnected();
    tcpSocket.open(QIODevice::ReadWrite);
    tcpSocket.reset();
}

/**
  * @brief Disconnect from the blackfin camera
  */
void bfcameracomms::server_disconnect()
{
    std::cout << "disconnect" << std::endl;
    tcpSocket.reset();
    tcpSocket.disconnectFromHost();
    timeTaken = 0;
    framesReceived = 0;
    fps = 0;
    // destroy objects
    delete image_buffer;

}

/**
  * @brief Send an image request to the blackfin camera
  */
void bfcameracomms::sendImageRequest()
{
    tcpSocket.write("I");
    tcpSocket.reset();
}

/**
  * @brief Receive an image from the blackfin camera
  */
void bfcameracomms::getImage()
{
    char* image_header_received = new char[10];
    quint8 temp = 0;
    int i = 0;
    int j = 0;
    // Wait until the image header, frame size, and frame byte size have been received (10 bytes)
    if (!header_received && (tcpSocket.bytesAvailable()>=10))
    {
        // read the first 10 bytes
        image_buffer->readRawData(image_header_received,10);
        // check for the image header
        for(i=0; i<abs(strlen(IMAGE_HEADER)); i++)
        {
            if (IMAGE_HEADER[i] == image_header_received[i])
                mark++;
        }
        if (mark == abs(strlen(IMAGE_HEADER)))
        {
            //std::cout << "> Image header received" << std::endl;
            // check for the frame pixel size
            frame_pixel_size = image_header_received[i];
            if (frame_pixel_size > 0)
            {
                frame_pixel_size_received = 1;
                //std::cout << "> Frame pixel size: " << frame_pixel_size << std::endl;
            }
            // calculate the frame byte size
            for(j=0; j<4; j++)
            {
                i++;
                temp = (quint8) image_header_received[i];
                frame_byte_size += temp*pow(256,j);
            }
            if (frame_byte_size >= 0)
            {
                //std::cout << "> Frame byte size: " << frame_byte_size << std::endl;
                header_received = 1;
            }
        }
        delete image_header_received;

    }

    // image header has been received, wait until the entire image has been sent
    if (header_received && (tcpSocket.bytesAvailable()>=frame_byte_size) && mark == 5)
    {
        mark = 0;
        //std::cout << "> Image received" << std::endl;
        // get the image data
        char* image = new char[frame_byte_size];
        image_buffer->readRawData(image,frame_byte_size);
        imarray = new QByteArray(image,frame_byte_size);
        delete image;
        tcpSocket.reset();
        emit SIG_imageReceived();
    }
}

/**
  * @brief Update the image and calculate fps
  */
void bfcameracomms::updateImage()
{
    // store the fps
    framesReceived++;
    timeTaken += t.elapsed();
    if (framesReceived >= 3)
    {
       fps = 3/(((qreal)timeTaken)/1000);
        framesReceived = 0;
        timeTaken = 0;
        //std::cout << ">> " << framesReceived << std::endl;
    }
    t.restart();
    // reset all variables
    header_received = 0;
    frame_pixel_size_received = 0;
    frame_byte_size_received = 0;
    frame_byte_size = 0;
    frame_pixel_size = 0;
    image_marker = 0;
    mark = 0;
    emit SIG_imageWritten();
    sendImageRequest();
}
