/********************************************************************************
** Form generated from reading ui file 'ahnsabout.ui'
**
** Created: Tue May 12 13:44:56 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_AHNSABOUT_H
#define UI_AHNSABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ahnsAbout
{
public:
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout1;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QFrame *frame_2;
    QHBoxLayout *hboxLayout;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem;
    QLabel *label_4;
    QSpacerItem *spacerItem1;
    QLabel *label_3;
    QSpacerItem *spacerItem2;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem3;
    QPushButton *OkBtn;

    void setupUi(QWidget *ahnsAbout)
    {
        if (ahnsAbout->objectName().isEmpty())
            ahnsAbout->setObjectName(QString::fromUtf8("ahnsAbout"));
        ahnsAbout->resize(440, 339);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ahnsAbout->sizePolicy().hasHeightForWidth());
        ahnsAbout->setSizePolicy(sizePolicy);
        vboxLayout = new QVBoxLayout(ahnsAbout);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setMargin(9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        vboxLayout1 = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout1->setMargin(0);
#endif
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        frame = new QFrame(ahnsAbout);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setMinimumSize(QSize(16, 100));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setMargin(9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);
        label_2->setMinimumSize(QSize(0, 80));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);


        vboxLayout1->addWidget(frame);

        frame_2 = new QFrame(ahnsAbout);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        sizePolicy1.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy1);
        frame_2->setMinimumSize(QSize(16, 10));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        hboxLayout = new QHBoxLayout(frame_2);
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setMargin(9);
#endif
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout1->setMargin(0);
#endif
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem);

        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);
        label_4->setPixmap(QPixmap(QString::fromUtf8(":/about/pic/SmartSkiesLogo.png")));

        hboxLayout1->addWidget(label_4);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);

        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);
        label_3->setPixmap(QPixmap(QString::fromUtf8(":/about/pic/arcaa_logo.bmp")));

        hboxLayout1->addWidget(label_3);

        spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem2);


        hboxLayout->addLayout(hboxLayout1);


        vboxLayout1->addWidget(frame_2);

        hboxLayout2 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout2->setSpacing(6);
#endif
        hboxLayout2->setMargin(0);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        spacerItem3 = new QSpacerItem(277, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem3);

        OkBtn = new QPushButton(ahnsAbout);
        OkBtn->setObjectName(QString::fromUtf8("OkBtn"));
        sizePolicy.setHeightForWidth(OkBtn->sizePolicy().hasHeightForWidth());
        OkBtn->setSizePolicy(sizePolicy);
        OkBtn->setMinimumSize(QSize(60, 0));

        hboxLayout2->addWidget(OkBtn);


        vboxLayout1->addLayout(hboxLayout2);


        vboxLayout->addLayout(vboxLayout1);


        retranslateUi(ahnsAbout);

        QMetaObject::connectSlotsByName(ahnsAbout);
    } // setupUi

    void retranslateUi(QWidget *ahnsAbout)
    {
        ahnsAbout->setWindowTitle(QApplication::translate("ahnsAbout", "About AHNS", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ahnsAbout", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:11pt;\">HELLO WORLD!</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_4->setText(QString());
        label_3->setText(QString());
        OkBtn->setText(QApplication::translate("ahnsAbout", "&OK", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ahnsAbout);
    } // retranslateUi

};

namespace Ui {
    class ahnsAbout: public Ui_ahnsAbout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AHNSABOUT_H
