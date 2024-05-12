#ifndef OMRONSIM_H
#define OMRONSIM_H

#include <QWidget>
#include <memory>
#include <QSettings>
#include <QGridLayout>
#include <QMap>
#include <QPushButton>

#include "VirtualOmron.h"
#include "OmronRepo.h"

namespace Ui {
class OmronSim;
}

namespace hw_omron = medicyc::cyclotroncontrolsystem::hardware::omron;

namespace medicyc::cyclotroncontrolsystem::simulators::omron {

class OmronSim : public QWidget
{
    Q_OBJECT

public:
    explicit OmronSim(VirtualOmron *omron, QWidget *parent = 0);
    ~OmronSim();

public slots:
    void ReadRequest(QString name);
    void WriteRequest(QString name, bool content);

signals:
    void SIGNAL_ReplyToReadRequest(QString name, bool content);
    void SIGNAL_ReplyToWriteRequest(QString name, bool content);

private slots:

    void SetupLayout();
    void SetupDatabase();

private:
    QString FrameStyleSheet(QString name, QString image) const;
    void FlashBackground(QPushButton *state);
    Ui::OmronSim *ui_;
    std::unique_ptr<hw_omron::OmronRepo> repo_;
    std::unique_ptr<QSettings> settings_;
    VirtualOmron *omron_;
    QMap<QString, QPushButton*> gridChannelMap_;
    QGridLayout *gridLayout_channels;
    void DisplayOmronChannels(const QMap<QString, hw_omron::Channel> channels);

    bool connected_ = false;
    enum class COLUMNS : int { SYSTEM = 0, NAME = 1, NODE = 2, AREA = 3, ADDRESS = 4, BIT = 5, IMPULS = 6, STATE = 7 };
};

}
#endif
