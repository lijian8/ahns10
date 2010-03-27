#ifndef AHPORTEXAMPLE_H
#define AHPORTEXAMPLE_H

#include <QtGui/QMainWindow>
#include <AH.h>

namespace Ui
{
    class AHPortExample;
}

class AHPortExample : public QMainWindow
{
    Q_OBJECT

public:
    AHPortExample(QWidget *parent = 0);
    ~AHPortExample();

private slots:
    void setRollRate(int updatedRollRate);
    void setPitchRate(int updatedPitchRate);
    void setAltitude(int updatedAltitude);

private:
    Ui::AHPortExample *ui;
    AHclass *AH_Core;

    /********* INITIALISATION FUNCTION DEFINITION *********/
        void Variable_Init();

    /******* ATTITUDE STATE VARIABLES ******/
        int bankRate;
        int pitchRate;
        int altPos;
};

#endif // AHPORTEXAMPLE_H
