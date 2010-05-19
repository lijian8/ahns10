/**
 * @file   ioimagehandler.h
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
 * Interface header to connect a opengl object and tcp connection object together
 *
 */

#ifndef IOIMAGEHANDLER_H
#define IOIMAGEHANDLER_H

#include <QObject>
#include <QDataStream>
#include <QBuffer>

class IOImageHandler : public QObject
{
    Q_OBJECT

    public:
        IOImageHandler() {}
        virtual ~IOImageHandler() {}

        QDataStream *image_buffer;
        QIODevice *theIODevice;
        QByteArray *imarray;
    public slots:
        virtual void getImage() = 0;

    signals:
        void SIG_imageWritten();

    protected:
    private:

};

#endif // IOIMAGEHANDLER_H
