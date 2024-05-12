#include "raspberry.h"
#include "ui_raspberry.h"

#include <QDebug>
#include <QPushButton>
#include <QStandardPaths>
#include <QMessageBox>

namespace medicyc::cyclotroncontrolsystem::hardware::raspberry {

RaspberryGui::RaspberryGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RaspberryGui)
{
    ui_->setupUi(this);

}

RaspberryGui::~RaspberryGui()
{
}

void RaspberryGui::Start() {
}

}

