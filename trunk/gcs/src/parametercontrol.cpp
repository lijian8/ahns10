/**
 * \file   parametercontrol.cpp
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
 * Implementation of the ParameterControl Widget.
 */

#include "parametercontrol.h"
#include "ui_parametercontrol.h"

#include "state.h"
#include "ahns_logger.h"

#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>

using namespace std;

ParameterControl::ParameterControl(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ParameterControl)
{
    ui->setupUi(this);
    on_loadBtn_clicked();
}

ParameterControl::~ParameterControl()
{
    delete ui;
}

void ParameterControl::changeEvent(QEvent *e)
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
QSize ParameterControl::SizeHint() const
{
    return QSize(420,270);
}


/**
  * @brief Handle resize of the widget
  */
void ParameterControl::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("ParameterControl::resizeEvent (QResizeEvent* e)");
    ui->layoutWidget->resize(e->size());
    return;
}

/**
  * @brief Slot to accept and display received parameters
  */
void ParameterControl::SetLoopParameters(const loop_parameters_t* const srcParameters)
{
    AHNS_DEBUG("void ParameterControl::SetLoopParameters(const loop_parameters_t* const srcParameters)");

    // Set Roll Boxes
    ui->rollMaxBox->setValue(srcParameters->rollMaximum);
    ui->rollMinBox->setValue(srcParameters->rollMinimum);
    ui->rollNeutralBox->setValue(srcParameters->rollNeutral);

    // Set Pitch Boxes
    ui->pitchMaxBox->setValue(srcParameters->pitchMaximum);
    ui->pitchMinBox->setValue(srcParameters->pitchMinimum);
    ui->pitchNeutralBox->setValue(srcParameters->pitchNeutral);

    // Set Yaw Boxes
    ui->yawMaxBox->setValue(srcParameters->yawMaximum);
    ui->yawMinBox->setValue(srcParameters->yawMinimum);
    ui->yawNeutralBox->setValue(srcParameters->yawNeutral);

    // Set x Boxes
    ui->xMaxBox->setValue(srcParameters->xMaximum);
    ui->xMinBox->setValue(srcParameters->xMinimum);
    ui->xNeutralBox->setValue(srcParameters->xNeutral);

    // Set y Boxes
    ui->yMaxBox->setValue(srcParameters->yMaximum);
    ui->yMinBox->setValue(srcParameters->yMinimum);
    ui->yNeutralBox->setValue(srcParameters->yNeutral);

    // Set z Boxes
    ui->zMaxBox->setValue(srcParameters->zMaximum);
    ui->zMinBox->setValue(srcParameters->zMinimum);
    ui->zNeutralBox->setValue(srcParameters->zNeutral);

    return;
}

/**
  * @brief Loads parameters from a user selected file on the GCS.
  */
void ParameterControl::on_loadBtn_clicked()
{
    AHNS_DEBUG("void ParameterControl::on_loadBtn_clicked()");

    int i = 0;
    int j = 0;

    QString fileName, msg;
    ifstream openFile;

    // User Choose File Name
    //fileName = QFileDialog::getOpenFileName(this, tr("Load Parameters"),QString(""),tr("AHNS Parameters (*.ahnsparam)"));
    fileName = QString("parameters.ahnsparam");

    if (!fileName.isEmpty())
    {
        openFile.open(fileName.toLatin1());

        if (!openFile.is_open())
        {
            AHNS_DEBUG("void ParameterControl::on_loadBtn_clicked() [ OPEN FAILED ]");
            msg = tr("Failed to open %1").arg(fileName);
            QMessageBox::warning(this, tr("Load Error"), msg);
        }
        else
        {
            double parameters[6][3];

            for (i = 0; i < 6; ++i)
            {
                for (j = 0; j < 3; ++j)
                {
                    openFile >> parameters[i][j];
                }
            }

            if (openFile.eof() || openFile.fail())
            {
                AHNS_DEBUG("void ParameterControl::on_loadBtn_clicked() [ READ FAILED ]");
                msg = tr("Failed to read %1").arg(fileName);
                QMessageBox::warning(this, tr("Read Error"), msg);
            }
            else
            {
                // Set Roll Boxes
                ui->rollMaxBox->setValue(parameters[0][0]);
                ui->rollNeutralBox->setValue(parameters[0][1]);
                ui->rollMinBox->setValue(parameters[0][2]);

                // Set Pitch Boxes
                ui->pitchMaxBox->setValue(parameters[1][0]);
                ui->pitchNeutralBox->setValue(parameters[1][1]);
                ui->pitchMinBox->setValue(parameters[1][2]);

                // Set Yaw Boxes
                ui->yawMaxBox->setValue(parameters[2][0]);
                ui->yawNeutralBox->setValue(parameters[2][1]);
                ui->yawMinBox->setValue(parameters[2][2]);

                // Set x Boxes
                ui->xMaxBox->setValue(parameters[3][0]);
                ui->xNeutralBox->setValue(parameters[3][1]);
                ui->xMinBox->setValue(parameters[3][2]);

                // Set y Boxes
                ui->yMaxBox->setValue(parameters[4][0]);
                ui->yNeutralBox->setValue(parameters[4][1]);
                ui->yMinBox->setValue(parameters[4][2]);

                // Set z Boxes
                ui->zMaxBox->setValue(parameters[5][0]);
                ui->zNeutralBox->setValue(parameters[5][1]);
                ui->zMinBox->setValue(parameters[5][2]);
            }
        }
    }
    return;
}

/**
  * @brief Save parameters on a user selected file on the GCS
  */
void ParameterControl::on_SaveBtn_clicked()
{
    AHNS_DEBUG("void ParameterControl::on_SaveBtn_clicked()");

    QString fileName, msg;
    ofstream saveFile;

    // User Choose File Name
    //fileName = QFileDialog::getSaveFileName(this, tr("Save Parameters"),QString(),tr("AHNS Parameters (*.ahnsparam)"));
    fileName = QString("parameters.ahnsparam");

    if (!fileName.isEmpty())
    {
        // Open File
        saveFile.open(fileName.toLatin1());

        if (!saveFile.is_open())
        {
            AHNS_DEBUG("void ParameterControl::on_SaveBtn_clicked() [ OPEN FAILED ]");
            msg = tr("Failed to open %1").arg(fileName);
            QMessageBox::warning(this, tr("Save Error"), msg);
        }
        else
        {
            // Output in 6 x 3 Matrix of Values
            saveFile << ui->rollMaxBox->value() << '\t' << ui->rollNeutralBox->value() << '\t' << ui->rollMinBox->value() << endl;
            saveFile << ui->pitchMaxBox->value() << '\t' << ui->pitchNeutralBox->value() << '\t' << ui->pitchMinBox->value() << endl;
            saveFile << ui->yawMaxBox->value() << '\t' << ui->yawNeutralBox->value() << '\t' << ui->yawMinBox->value() << endl;

            saveFile << ui->xMaxBox->value() << '\t' << ui->xNeutralBox->value() << '\t' << ui->xMinBox->value() << endl;
            saveFile << ui->yMaxBox->value() << '\t' << ui->yNeutralBox->value() << '\t' << ui->yMinBox->value() << endl;
            saveFile << ui->zMaxBox->value() << '\t' << ui->zNeutralBox->value() << '\t' << ui->zMinBox->value() << endl;

            if (saveFile.fail())
            {
                AHNS_DEBUG("void ParameterControl::on_SaveBtn_clicked() [ SAVE FAILED ]");
                msg = tr("Error writing to %1") .arg(fileName);
                QMessageBox::warning(this, tr("Save Error"), msg);
            }
            saveFile.close();
        }
    }
    return;
}

/**
  * @brief Send the Parameters in the GCS to the Flight Computer
  */
void ParameterControl::on_SendBtn_clicked()
{
    AHNS_DEBUG("void ParameterControl::on_SendBtn_clicked()");

    loop_parameters_t loopConfig;

    // Set Roll Parameters
    loopConfig.rollMaximum = ui->rollMaxBox->value();
    loopConfig.rollMinimum = ui->rollMinBox->value();
    loopConfig.rollNeutral = ui->rollNeutralBox->value();

    // Set Pitch Parameters
    loopConfig.pitchMaximum = ui->pitchMaxBox->value();
    loopConfig.pitchMinimum = ui->pitchMinBox->value();
    loopConfig.pitchNeutral = ui->pitchNeutralBox->value();

    // Set Yaw Parameters
    loopConfig.yawMaximum = ui->yawMaxBox->value();
    loopConfig.yawMinimum = ui->yawMinBox->value();
    loopConfig.yawNeutral = ui->yawNeutralBox->value();

    // Set x Parameters
    loopConfig.xMaximum = ui->xMaxBox->value();
    loopConfig.xMinimum = ui->xMinBox->value();
    loopConfig.xNeutral = ui->xNeutralBox->value();

    // Set y Parameters
    loopConfig.yMaximum = ui->yMaxBox->value();
    loopConfig.yMinimum = ui->yMinBox->value();
    loopConfig.yNeutral = ui->yNeutralBox->value();

    // Set z Parameters
    loopConfig.zMaximum = ui->zMaxBox->value();
    loopConfig.zMinimum = ui->zMinBox->value();
    loopConfig.zNeutral = ui->zNeutralBox->value();

    emit sendParameters(loopConfig);
    return;
}
