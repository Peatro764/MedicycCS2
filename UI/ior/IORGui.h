#ifndef IORGui_H
#define IORGui_H

#include <QWidget>
#include <vector>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>

#include "IOR.h"
#include "IORRepo.h"

namespace Ui {
class IORGui;
}

namespace hw_ior = medicyc::cyclotroncontrolsystem::hardware::ior;

namespace medicyc::cyclotroncontrolsystem::ui::ior {

class IORGui : public QWidget
{
    Q_OBJECT

public:
    explicit IORGui(QWidget *parent = 0);
    ~IORGui();

public slots:
    void SetCardAddresses();
    void SetPartialScan();
    void WriteBlockMask(int card, int block);
    void ProcessChannelValue(QString channel, bool value);
    void ProcessChannelMask(QString channel, bool value);

private slots:
    void ShowPing();
    void ShowConnected();
    void ShowDisconnected();

private:
    void SetupDatabase();
    void SetupLayout();
    void DisplayUIChannels(const std::vector<hw_ior::Channel>& channels);
    std::vector<hw_ior::Channel> GetIORChannels();
    quint8 GetBlockMask(int card, int block) const;
    void FlashBackground(QLabel *label);
    void FlashBackground(QLineEdit *lineEdit);
    QString FrameStyleSheet(QString name, QString image) const;

    Ui::IORGui *ui_;
    std::unique_ptr<QSettings> settings_;
    hw_ior::IOR ior_;
    std::unique_ptr<hw_ior::IORRepo> repo_;
    QGridLayout *gridLayout_channels;
    QMap<QString, QLabel*> channel_value_label_;
    QMap<QString, QLineEdit*> channel_mask_label_;

    enum class COLUMNS : int { CHANNEL = 0, BIT = 1, NAME = 2, VALUE = 3, READ_VALUE = 4, MASK = 5, READ_MASK = 6, WRITE_MASK = 7, SPACER = 8 };
    const int NCARDS = 4;
    const int NCOLSPERBIT = 9;
    const int NBLOCKSPERCARD = 3;
    const int NBITSPERBLOCK = 8;

};

}

#endif
