/**
 * \file   gainscontrol.cpp
 * \author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Implementation of the GainsControl Widget.
 */

#include "gainscontrol.h"
#include "ui_gainscontrol.h"

#include <QSize>
#include <QResizeEvent>

#include "state.h"
#include "ahns_logger.h"

#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>

using namespace std;

GainsControl::GainsControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GainsControl)
{
    ui->setupUi(this);
}

GainsControl::~GainsControl()
{
    delete ui;
}

void GainsControl::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * @brief SizeHint resize
  */
QSize GainsControl::SizeHint() const
{
    return QSize(420,270);
}


/**
  * @brief Handle resize of the widget
  */
void GainsControl::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("GainsControl::resizeEvent (QResizeEvent* e)");
    ui->layoutWidget->resize(e->size());
    return;
}


/**
  * @breif Slot to accept and display received gains
  */
void GainsControl::SetGains(gains_t const* srcGains)
{
    AHNS_DEBUG("void GainsControl::SetGains(gains_t const* srcGains)");

    // Roll Gains
    ui->rollPBox->setValue(srcGains->rollKp);
    ui->rollIBox->setValue(srcGains->rollKi);
    ui->rollDBox->setValue(srcGains->rollKd);

    // Pitch Gains
    ui->pitchPBox->setValue(srcGains->pitchKp);
    ui->pitchIBox->setValue(srcGains->pitchKi);
    ui->pitchDBox->setValue(srcGains->pitchKd);

    // Yaw Gains
    ui->yawPBox->setValue(srcGains->yawKp);
    ui->yawIBox->setValue(srcGains->yawKi);
    ui->yawDBox->setValue(srcGains->yawKd);

    // x Gains
    ui->xPBox->setValue(srcGains->xKp);
    ui->xIBox->setValue(srcGains->xKi);
    ui->xDBox->setValue(srcGains->xKd);

    // y Gains
    ui->yPBox->setValue(srcGains->yKp);
    ui->yIBox->setValue(srcGains->yKi);
    ui->yDBox->setValue(srcGains->yKd);

    // z Gains
    ui->zPBox->setValue(srcGains->zKp);
    ui->zIBox->setValue(srcGains->zKi);
    ui->zDBox->setValue(srcGains->zKd);

    return;
}

/**
  * @brief Load gains from user selected GCS file
  */
void GainsControl::on_loadBtn_clicked()
{
    AHNS_DEBUG("void GainsControl::on_loadBtn_clicked()");

    int i = 0;
    int j = 0;

    QString fileName, msg;
    ifstream openFile;

    // User Choose File Name
    //fileName = QFileDialog::getOpenFileName(this, tr("Load Gains"),QString(""),tr("AHNS Gains (*.ahnsgains)"));
    fileName = QString("gains.ahnsgains");

    if (!fileName.isEmpty())
    {
        openFile.open(fileName.toLatin1());

        if (!openFile.is_open())
        {
            AHNS_DEBUG("void GainsControl::on_loadBtn_clicked() [ OPEN FAILED ]");
            msg = tr("Failed to open %1").arg(fileName);
            QMessageBox::warning(this, tr("Load Error"), msg);
        }
        else
        {
            double gains[6][3];

            for (i = 0; i < 6; ++i)
            {
                for (j = 0; j < 3; ++j)
                {
                    openFile >> gains[i][j];
                }
            }

            if (openFile.eof() || openFile.fail())
            {
                AHNS_DEBUG("void GainsControl::on_loadBtn_clicked() [ READ FAILED ]");
                msg = tr("Failed to read %1").arg(fileName);
                QMessageBox::warning(this, tr("Read Error"), msg);
            }
            else
            {
                // Set Roll Boxes
                ui->rollPBox->setValue(gains[0][0]);
                ui->rollIBox->setValue(gains[0][1]);
                ui->rollDBox->setValue(gains[0][2]);

                // Set Pitch Boxes
                ui->pitchPBox->setValue(gains[1][0]);
                ui->pitchIBox->setValue(gains[1][1]);
                ui->pitchDBox->setValue(gains[1][2]);

                // Set Yaw Boxes
                ui->yawPBox->setValue(gains[2][0]);
                ui->yawIBox->setValue(gains[2][1]);
                ui->yawDBox->setValue(gains[2][2]);

                // Set x Boxes
                ui->xPBox->setValue(gains[3][0]);
                ui->xIBox->setValue(gains[3][1]);
                ui->xDBox->setValue(gains[3][2]);

                // Set y Boxes
                ui->yPBox->setValue(gains[4][0]);
                ui->yIBox->setValue(gains[4][1]);
                ui->yDBox->setValue(gains[4][2]);

                // Set z Boxes
                ui->zPBox->setValue(gains[5][0]);
                ui->zIBox->setValue(gains[5][1]);
                ui->zDBox->setValue(gains[5][2]);
            }
        }
    }
    return;
}

/**
  * @brief Save GCS gains to user selected file
  */
void GainsControl::on_SaveBtn_clicked()
{
    AHNS_DEBUG("void GainsControl::on_SaveBtn_clicked()");

    QString fileName, msg;
    ofstream saveFile;

    // User Choose File Name
    fileName = QFileDialog::getSaveFileName(this, tr("Save Gains"),QString(),tr("AHNS Gains (*.ahnsgains)"));
    fileName.append(".ahnsgains");

    if (!fileName.isEmpty())
    {
        // Open File
        saveFile.open(fileName.toLatin1());

        if (!saveFile.is_open())
        {
            AHNS_DEBUG("void GainsControl::on_SaveBtn_clicked() [ OPEN FAILED ]");
            msg = tr("Failed to open %1").arg(fileName);
            QMessageBox::warning(this, tr("Save Error"), msg);
        }
        else
        {
            // Output in 6 x 3 Matrix of Values
            saveFile << ui->rollPBox->value() << '\t' << ui->rollIBox->value() << '\t' << ui->rollDBox->value() << endl;
            saveFile << ui->pitchPBox->value() << '\t' << ui->pitchIBox->value() << '\t' << ui->pitchDBox->value() << endl;
            saveFile << ui->yawPBox->value() << '\t' << ui->yawIBox->value() << '\t' << ui->yawDBox->value() << endl;

            saveFile << ui->xPBox->value() << '\t' << ui->xIBox->value() << '\t' << ui->xDBox->value() << endl;
            saveFile << ui->yPBox->value() << '\t' << ui->yIBox->value() << '\t' << ui->yDBox->value() << endl;
            saveFile << ui->zPBox->value() << '\t' << ui->zIBox->value() << '\t' << ui->zDBox->value() << endl;

            if (saveFile.fail())
            {
                AHNS_DEBUG("void GainsControl::on_SaveBtn_clicked() [ SAVE FAILED ]");
                msg = tr("Error writing to %1") .arg(fileName);
                QMessageBox::warning(this, tr("Save Error"), msg);
            }
            saveFile.close();
        }
    }

    return;
}

/**
  * @brief Send Gains from GCS to FC
  */
void GainsControl::on_SendBtn_clicked()
{
    AHNS_DEBUG("void GainsControl::on_SendBtn_clicked()");

    gains_t loopGains;

    // Roll Gains
    loopGains.rollKp = ui->rollPBox->value();
    loopGains.rollKi = ui->rollIBox->value();
    loopGains.rollKd = ui->rollDBox->value();

    // Pitch Gains
    loopGains.pitchKp = ui->pitchPBox->value();
    loopGains.pitchKi = ui->pitchIBox->value();
    loopGains.pitchKd = ui->pitchDBox->value();

    // Yaw Gains
    loopGains.yawKp = ui->yawPBox->value();
    loopGains.yawKi = ui->yawIBox->value();
    loopGains.yawKd = ui->yawDBox->value();

    // x Gains
    loopGains.xKp = ui->xPBox->value();
    loopGains.xKi = ui->xIBox->value();
    loopGains.xKd = ui->xDBox->value();

    // y Gains
    loopGains.yKp = ui->yPBox->value();
    loopGains.yKi = ui->yIBox->value();
    loopGains.yKd = ui->yDBox->value();

    // z Gains
    loopGains.zKp = ui->zPBox->value();
    loopGains.zKi = ui->zIBox->value();
    loopGains.zKd = ui->zDBox->value();

    emit sendGains(loopGains);

    return;
}
