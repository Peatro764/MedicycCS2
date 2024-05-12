#ifndef TESTS_H
#define TESTS_H


#include <QMainWindow>
#include "ui_tests.h"

class tests : public QMainWindow
{
    Q_OBJECT

public:

    tests(QWidget *parent = 0);
    ~tests();

public slots:

private slots:

private:

    Ui::tests *ui_;
};

#endif
