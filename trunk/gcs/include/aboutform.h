#ifndef ABOUTFORM_H
#define ABOUTFORM_H

#include <QWidget>

namespace Ui {
    class aboutForm;
}

class aboutForm : public QWidget {
    Q_OBJECT
public:
    aboutForm(QWidget *parent = 0);
    ~aboutForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::aboutForm *ui;
};

#endif // ABOUTFORM_H
