#ifndef IORSim_H
#define IORSim_H

#include <QWidget>
#include <vector>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

#include "VirtualIOR.h"
#include "Channel.h"
#include "IORRepo.h"

namespace Ui {
class IORSim;
}

namespace hw_ior = medicyc::cyclotroncontrolsystem::hardware::ior;

namespace medicyc::cyclotroncontrolsystem::simulators::ior {

class IORSim : public QWidget
{
    Q_OBJECT

public:
    explicit IORSim(VirtualIOR *ior, QWidget *parent = 0);
    ~IORSim();

public slots:
    void ReadValueRequest(QString name);
    void ReadMaskRequest(QString name);

signals:
    void SIGNAL_ReplyToReadValueRequest(QString name, bool state);
    void SIGNAL_ReplyToReadMaskRequest(QString name, bool state);

private slots:
    void ShowConnected();
    void ShowDisconnected();

private:
    void SetupDatabase();
    void SetupLayout();
    void DisplayUIChannels(const std::vector<hw_ior::Channel>& channels);
    std::vector<hw_ior::Channel> GetIORChannels();
    quint8 GetBlockMask(int card, int block) const;
    void FlashBackground(QPushButton *button);
    QString FrameStyleSheet(QString name, QString image) const;

    Ui::IORSim *ui_;
    std::unique_ptr<QSettings> settings_;
    VirtualIOR *ior_;
    std::unique_ptr<hw_ior::IORRepo> repo_;
    QGridLayout *gridLayout_channels;
    QMap<QString, QPushButton*> channel_value_button_;
    QMap<QString, QPushButton*> channel_mask_button_;

    enum class COLUMNS : int { CHANNEL = 0, BIT = 1, NAME = 2, VALUE = 3, MASK = 4, SPACER = 5 };
    const int NCARDS = 4;
    const int NCOLSPERBIT = 6;
    const int NBLOCKSPERCARD = 3;
    const int NBITSPERBLOCK = 8;

};

}

#endif
