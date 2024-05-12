#ifndef OMRONGUI_H
#define OMRONGUI_H

#include <QWidget>
#include <memory>
#include <QSettings>
#include <QGridLayout>
#include <QMap>
#include <QPushButton>

#include "Omron.h"
#include "OmronRepo.h"

namespace Ui {
class OmronGui;
}

namespace medicyc::cyclotroncontrolsystem::ui::omron {

namespace hw_omron = medicyc::cyclotroncontrolsystem::hardware::omron;

class OmronGui : public QWidget
{
    Q_OBJECT

public:
     OmronGui(QString system);
    ~OmronGui();

public slots:
    void ReadBit();
    void ReadWord();
    void WriteBit();
    void WriteWord();

private slots:
    void ShowPing();
    void ShowConnected();
    void ShowDisconnected();
    void DisplayFINSHeader(medicyc::cyclotroncontrolsystem::hardware::omron::FINSHeader header);
    void DisplayWordRead(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content);
    void DisplayBitRead(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, bool content);
    void DisplayWordWritten(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content);
    void DisplayBitWritten(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, bool content);

    void DisplayStateMachineMessage(QString message);
    void DisplayIOError(QString message);

    void SetupLayout();
    void SetupDatabase();
    void ReadChannel(QString name);
    void WriteChannel(QString name, QPushButton* value);

private:
    void FillComboBoxes();
    QString FrameStyleSheet(QString name, QString image) const;
    void FlashBackground(QPushButton *state);
    Ui::OmronGui *ui_;
    hw_omron::Omron omron_;
    std::unique_ptr<hw_omron::OmronRepo> repo_;

    QGridLayout *gridLayout_channels;
    void DisplayOmronChannels(const QMap<QString, medicyc::cyclotroncontrolsystem::hardware::omron::Channel> channels);

    enum class COLUMNS : int { SYSTEM = 0, NAME = 1, NODE = 2, AREA = 3, ADDRESS = 4, BIT = 5, IMPULS = 6, READ = 7, WRITE = 8, STATE = 9, PING = 10 };
};

}
#endif
