#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_div.h>
#include <qwt_legend.h>
#include "include/GUIofficial.h"
#include "ui_UDPconnection.h"
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <QPen>

using namespace std;

void GUIofficial::Plots_Init()
{
	db_altTimer = new QTimer(this);
	
	connect(pl_alt_histBar, SIGNAL(valueChanged(int)), this, SLOT(plots_alt_seekHistory()));
	connect(pl_altPID_histBar, SIGNAL(valueChanged(int)), this, SLOT(plots_altPID_seekHistory()));
	//connect(db_altTimer, SIGNAL(timeout()), this, SLOT(db_updatePlot()));

	count = 0;
	reScale = 0;
	altClear = false;
	network_firstRun = true;
	isPlotRunning = false;

	plots_formatPlots();
	pl_alt->setAutoReplot();
	pl_alt_PID->setAutoReplot();
	//pl_alt->setAxisScale(2, 0, 10); // X Axis ID: 2
	//pl_alt->setAxisScale(0, 0, 220);  // Y Axis ID: 0
	//QwtScaleDiv *scale = new QwtScaleDiv();
	//scale->setInterval(400.0, 600.0);

	//db_altTimer->start(40); // Updateing at 25Hz
}

void GUIofficial::plots_formatPlots()
{
	QFont axisTitleFont, plotTitleFont;
	axisTitleFont.setPointSize(10);
	plotTitleFont.setPointSize(11);
	
	QwtText altText("ALT (cm)");
	QwtText pidText("PID Collective");
	QwtText timeText("Time (secs)");
	QwtText altTitle("ALTITUDE PLOT");
	QwtText altPidTitle("PID COMPONENTS");
	
	altText.setFont(axisTitleFont);
	timeText.setFont(axisTitleFont);
	pidText.setFont(axisTitleFont);
	altTitle.setFont(plotTitleFont);
	altPidTitle.setFont(plotTitleFont);
	
	//axisTitleFont.setPointSize(7);
	//pl_alt->setAxisFont(2, axisTitleFont); // X Axis ID: 2
	//pl_alt->setAxisFont(0, axisTitleFont); // Y Axis ID: 0
	
	/* ******** Live Update Altitude Plot ******* */
	pl_alt->setAxisTitle(2, timeText);
	pl_alt->setTitle(altTitle);
	pl_alt_PID->setAxisTitle(2, timeText);
	pl_alt_PID->setTitle(altPidTitle);
	pl_alt->setAxisTitle(0, altText);
	pl_alt_PID->setAxisTitle(0, pidText);
	
	pl_alt->setAxisScale(2, 0, 10); // X Axis ID: 2
	pl_alt->setAxisScale(0, 0, 300); // Y Axis ID: 0
	pl_alt_PID->setAxisScale(2, 0, 10); // X Axis ID: 2
	pl_alt_PID->setAxisScale(0, -22, 22); // Y Axis ID: 0
	
	/* ****** ASSIGN PENS STYLES ****** */
	QPen cyanPen(QColor(103, 200, 255));	//cyanPen.setWidth(2);
	QPen redPen(QColor(255, 0, 0));		redPen.setStyle(Qt::DashLine);
	QPen greenPen(QColor(0, 255, 0));	greenPen.setStyle(Qt::DotLine);
	QPen yellowPen(QColor(255, 255, 0));	yellowPen.setStyle(Qt::DashLine);
	QPen pinkPen(QColor(255, 0, 255));	pinkPen.setStyle(Qt::DashDotLine);
	
	/* ****** ASSIGN PENS TO CURVES ****** */
	qpc_altCMD.setPen(redPen);
	qpc_altTRU.setPen(cyanPen);
	qpc_altPID.setPen(cyanPen);
	qpc_altPID_P.setPen(greenPen);
	qpc_altPID_I.setPen(yellowPen);
	qpc_altPID_D.setPen(pinkPen);
}

void GUIofficial::plots_updatePlot(receiverPacket_AP packBuffer)
{
	isPlotRunning = true;
	if (altClear)
	{
		reScale = 0;
		count = 0;
		altClear = false;
		vec_plotAlt.clear();
		vec_plotAlt_PID.clear();
		pl_alt->setAxisScale(2, 0, 10); // X Axis ID: 2
		pl_alt_PID->setAxisScale(2, 0, 10); // X Axis ID: 2
		pl_alt_histBar->setMaximum(50);
		pl_alt_histBar->setValue(50);
		pl_altPID_histBar->setMaximum(50);
		pl_altPID_histBar->setValue(50);
		displaySystemMessage_NORMAL(PLOTS_CLEAR);
	}

	++reScale;
	double alt_t[reScale];
	double altPID_t[reScale];
	double alt_CMD[reScale];
	double alt_TRU[reScale];
	double alt_PID[reScale];
	double alt_P[reScale];
	double alt_I[reScale];
	double alt_D[reScale];

	/* ****** ASSIGN TEMPORARY BUFFER ****** */
	double tempHold_t = (double)reScale/25;
	/*double tempHold_altCMD = db_myPlotSlider_1->value(); // 
	double tempHold_altTRU = db_myPlotSlider_2->value(); //
	double tempHold_alt_P = db_plotSlider_P->value();
	double tempHold_alt_I = db_plotSlider_I->value();
	double tempHold_alt_D = db_plotSlider_D->value();*/
	double tempHold_altCMD = (double)(((packBuffer.altCMD_HI << (3*8)) | (packBuffer.altCMD_MH << (2*8)) | (packBuffer.altCMD_ML << (1*8)) | (packBuffer.altCMD_LO << (0*8)))/10.0);
	double tempHold_altTRU = (double)(((packBuffer.altTRU_HI << (3*8)) | (packBuffer.altTRU_MH << (2*8)) | (packBuffer.altTRU_ML << (1*8)) | (packBuffer.altTRU_LO << (0*8)))/10.0);
	double tempHold_alt_P = (double)(((packBuffer.altP_HI << (3*8)) | (packBuffer.altP_MH << (2*8)) | (packBuffer.altP_ML << (1*8)) | (packBuffer.altP_LO << (0*8)))/100.0);
	double tempHold_alt_I = (double)(((packBuffer.altI_HI << (3*8)) | (packBuffer.altI_MH << (2*8)) | (packBuffer.altI_ML << (1*8)) | (packBuffer.altI_LO << (0*8)))/100.0);
	double tempHold_alt_D = (double)(((packBuffer.altD_HI << (3*8)) | (packBuffer.altD_MH << (2*8)) | (packBuffer.altD_ML << (1*8)) | (packBuffer.altD_LO << (0*8)))/100.0);
	double tempHold_alt_PID = (double)(((packBuffer.altPID_HI << (3*8)) | (packBuffer.altPID_MH << (2*8)) | (packBuffer.altPID_ML << (1*8)) | (packBuffer.altPID_LO << (0*8)))/100.0);

	/* ****** ALLOCATING NEW DATA TO VECTOR ****** */
	vec_plotAlt.push_back(tempHold_t);		// Allocate new time
	vec_plotAlt.push_back(tempHold_altCMD);		// Allocate new commanded altitude
	vec_plotAlt.push_back(tempHold_altTRU);		// Allocate new true altitude
	vec_plotAlt_PID.push_back(tempHold_t);		// Allocate new time
	vec_plotAlt_PID.push_back(tempHold_alt_P);	// Allocate new P command
	vec_plotAlt_PID.push_back(tempHold_alt_I);	// Allocate new I command
	vec_plotAlt_PID.push_back(tempHold_alt_D);	// Allocate new D command
	vec_plotAlt_PID.push_back(tempHold_alt_PID);	// Allocate new PID command

	/* ****** ASSIGNING NEW ARRAY EVERY TIME ****** */
	for (int ii=0; ii<(signed int)vec_plotAlt.size(); ii+=3)
	{
		alt_t[ii/3] = vec_plotAlt.at(ii);
		alt_CMD[ii/3] = vec_plotAlt.at(ii+1);
		alt_TRU[ii/3] = vec_plotAlt.at(ii+2);
	}
	
	for (int ii=0; ii<(signed int)vec_plotAlt_PID.size(); ii+=5)
	{
		altPID_t[ii/5] = vec_plotAlt_PID.at(ii);
		alt_P[ii/5] = vec_plotAlt_PID.at(ii+1);
		alt_I[ii/5] = vec_plotAlt_PID.at(ii+2);
		alt_D[ii/5] = vec_plotAlt_PID.at(ii+3);
		alt_PID[ii/5] = vec_plotAlt_PID.at(ii+4);
	}

	if (alt_t[reScale-1] == (10 + 5*count))
	{
		count++;
		pl_alt_histBar->setMaximum(alt_t[reScale-1]*10);
		pl_alt_histBar->setValue(alt_t[reScale-1]*10);
		pl_altPID_histBar->setMaximum(altPID_t[reScale-1]*10);
		pl_altPID_histBar->setValue(altPID_t[reScale-1]*10);
	}

	qpc_altCMD.setData(alt_t, alt_CMD, reScale);
	qpc_altTRU.setData(alt_t, alt_TRU, reScale);
	qpc_altPID.setData(altPID_t, alt_PID, reScale);
	qpc_altPID_P.setData(altPID_t, alt_P, reScale);
	qpc_altPID_I.setData(altPID_t, alt_I, reScale);
	qpc_altPID_D.setData(altPID_t, alt_D, reScale);

	qpc_altCMD.attach(pl_alt);
	qpc_altTRU.attach(pl_alt);
	qpc_altPID.attach(pl_alt_PID);
	qpc_altPID_P.attach(pl_alt_PID);
	qpc_altPID_I.attach(pl_alt_PID);
	qpc_altPID_D.attach(pl_alt_PID);
	
	pl_alt->autoRefresh();
	pl_alt_PID->autoRefresh();
	
	isPlotRunning = false;
}

void GUIofficial::plots_alt_seekHistory()
{
	double reqWindow_LL = (pl_alt_histBar->value())*0.1;
	pl_alt->setAxisScale(2, (double)reqWindow_LL-5, (double)(reqWindow_LL+5));
	pl_altPID_histBar->setValue(pl_alt_histBar->value());
	
	qpc_altCMD.attach(pl_alt);
	qpc_altTRU.attach(pl_alt);
	qpc_altPID.attach(pl_alt_PID);
	qpc_altPID_P.attach(pl_alt_PID);
	qpc_altPID_I.attach(pl_alt_PID);
	qpc_altPID_D.attach(pl_alt_PID);
}

void GUIofficial::plots_altPID_seekHistory()
{
	double reqWindow_LL = (pl_altPID_histBar->value())*0.1;
	pl_alt_PID->setAxisScale(2, (double)reqWindow_LL-5, (double)(reqWindow_LL+5));
	pl_alt_histBar->setValue(pl_altPID_histBar->value());
}

void GUIofficial::on_plot_btnAltClear_clicked()
{
	altClear = true;
	if (!isPlotRunning)
	{
		double plot_clear[1];
		plot_clear[0] = 0;
		qpc_altCMD.setData(plot_clear, plot_clear, 1);
		qpc_altTRU.setData(plot_clear, plot_clear, 1);
		qpc_altPID.setData(plot_clear, plot_clear, 1);
		qpc_altPID_P.setData(plot_clear, plot_clear, 1);
		qpc_altPID_I.setData(plot_clear, plot_clear, 1);
		qpc_altPID_D.setData(plot_clear, plot_clear, 1);

		qpc_altCMD.attach(pl_alt);
		qpc_altTRU.attach(pl_alt);
		qpc_altPID.attach(pl_alt_PID);
		qpc_altPID_P.attach(pl_alt_PID);
		qpc_altPID_I.attach(pl_alt_PID);
		qpc_altPID_D.attach(pl_alt_PID);
		
		pl_alt->setAxisScale(2, 0, 10); // X Axis ID: 2
		pl_alt_PID->setAxisScale(2, 0, 10); // X Axis ID: 2
		pl_alt_histBar->setMaximum(50);
		pl_alt_histBar->setValue(50);
		pl_altPID_histBar->setMaximum(50);
		pl_altPID_histBar->setValue(50);
		displaySystemMessage_NORMAL(PLOTS_CLEAR);
	}
}



