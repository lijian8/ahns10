#include "ahportexample.h"
#include "ui_ahportexample.h"
#include <AH.h>

AHPortExample::AHPortExample(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AHPortExample)
{
    ui->setupUi(this);

    Variable_Init();
    AH_Core = new AHclass;
    AH_Core->show();
    ui->scrollArea->setWidget(AH_Core);
}

AHPortExample::~AHPortExample()
{
    delete ui;
}

void AHPortExample::Variable_Init()
{
    bankRate = 0;
    pitchRate = 0;
    altPos = 0;

    return;
}

void AHPortExample::setRollRate(int updatedRollRate)
{
    AH_Core->angRoll = (float) updatedRollRate;
    AH_Core->UpdateRoll();

}

void AHPortExample::setPitchRate(int updatedPitchRate)
{
    AH_Core->angPitch = (float) -updatedPitchRate*3.15192/180.0;;
    AH_Core->UpdateRoll();
}

void AHPortExample::setAltitude(int updatedAltitude)
{
    AH_Core->altState = (float)(updatedAltitude);
    AH_Core->UpdateRoll();
}

void AHPortExample::zeroRoll()
{
    AH_Core->angRoll = 0;
    AH_Core->UpdateRoll();
    ui->horizontalScrollBar->setValue(0);
}

void AHPortExample::zeroPitch()
{
    AH_Core->angPitch = 0;
    AH_Core->UpdateRoll();
    ui->horizontalScrollBar_2->setValue(0);
}

void AHPortExample::zeroAlt()
{
    AH_Core->altState = 0;
    AH_Core->UpdateRoll();
    ui->horizontalScrollBar_3->setValue(0);
}
