/**
 * @file   receiveconsole.h
 * @author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Receive Console widget to display the packets received and disgarded by the GCS
 */

#ifndef RECEIVECONSOLE_H
#define RECEIVECONSOLE_H

#include <QWidget>

namespace Ui {
    class ReceiveConsole;
}

class ReceiveConsole : public QWidget {
    Q_OBJECT
public:
    ReceiveConsole(QWidget *parent = 0);
    ~ReceiveConsole();

    void addItem(QString item, const bool discarded);
    bool receivedShow() const;
    bool discardedShow() const;
    bool detailShow() const;

    void clearConsole();

    QSize sizeHint() const;

public slots:
    void RxSpeed(const double& linkSpeed);

protected:
    void changeEvent(QEvent *e);
    void resizeEvent (QResizeEvent* e);

private:
    Ui::ReceiveConsole *ui;

    quint64 m_packetCount;
    quint64 m_discardedCount;
};

#endif // RECEIVECONSOLE_H
