#include <QtGui/QApplication>
#include "ahportexample.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AHPortExample w;
    w.show();
    return a.exec();
}
