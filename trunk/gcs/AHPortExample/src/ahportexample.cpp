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
    AH_Core->angRollRate = (float)(((updatedRollRate/50.0f)*30)/50);
    AH_Core->UpdateRoll();

}

void AHPortExample::setPitchRate(int updatedPitchRate)
{
    AH_Core->angPitchRate = (float)(((-updatedPitchRate/50.0f)*10)/50);
    AH_Core->UpdateRoll();
}

void AHPortExample::setAltitude(int updatedAltitude)
{
    AH_Core->vertAltPos = (float)(updatedAltitude);
    AH_Core->UpdateRoll();
}
