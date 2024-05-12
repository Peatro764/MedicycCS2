#include "tests.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);
    tests t;
    return a.exec();
}

