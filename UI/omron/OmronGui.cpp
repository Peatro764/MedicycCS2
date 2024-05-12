#include "OmronGui.h"
#include "ui_OmronGui.h"

#include <QMessageBox>
#include <QStandardPaths>
#include <QDebug>
#include <unistd.h>
#include <QtGlobal>

namespace medicyc::cyclotroncontrolsystem::ui::omron {

OmronGui::OmronGui(QString system) :
    QWidget(nullptr),
    ui_(new Ui::OmronGui)
{
    ui_->setupUi(this);
    SetupLayout();
    setWindowTitle(system);

    SetupDatabase();
    auto channels = repo_->GetChannels(system);
    qDebug() << "Number of channels " << channels.size();
    qDebug() << channels.keys();
    omron_.SetChannels(channels);
    DisplayOmronChannels(channels);

    assert(QObject::connect(&omron_, SIGNAL(SIGNAL_BitRead(uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint8_t, bool)), this, SLOT(DisplayBitRead(uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint8_t, bool))));
    assert(QObject::connect(&omron_, SIGNAL(SIGNAL_WordRead(uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint16_t)), this, SLOT(DisplayWordRead(uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint16_t))));
    assert(QObject::connect(&omron_, SIGNAL(SIGNAL_BitWritten(uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint8_t, bool)), this, SLOT(DisplayBitWritten(uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint8_t, bool))));
    assert(QObject::connect(&omron_, SIGNAL(SIGNAL_WordWritten(uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint16_t)), this, SLOT(DisplayWordWritten(uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint16_t))));

    QObject::connect(ui_->pushButton_read_bit, &QPushButton::clicked, this, &OmronGui::ReadBit);
    QObject::connect(ui_->pushButton_read_word, &QPushButton::clicked, this, &OmronGui::ReadWord);

    QObject::connect(ui_->pushButton_write_bit, &QPushButton::clicked, this, &OmronGui::WriteBit);
    QObject::connect(ui_->pushButton_write_word, &QPushButton::clicked, this, &OmronGui::WriteWord);

    assert(QObject::connect(&omron_, SIGNAL(SIGNAL_Connected()), this, SLOT(ShowConnected())));
    assert(QObject::connect(&omron_, SIGNAL(SIGNAL_Disconnected()), this, SLOT(ShowDisconnected())));
    assert(QObject::connect(&omron_, SIGNAL(SIGNAL_ReplyReceived()) , this, SLOT(ShowPing())));

    assert(QObject::connect(&omron_, SIGNAL(SIGNAL_NodeAddressReceived(medicyc::cyclotroncontrolsystem::hardware::omron::FINSHeader)), this, SLOT(DisplayFINSHeader(medicyc::cyclotroncontrolsystem::hardware::omron::FINSHeader))));

    FillComboBoxes();
    QTimer::singleShot(1000, &omron_, &medicyc::cyclotroncontrolsystem::hardware::omron::Omron::ReadAllChannels);
}

OmronGui::~OmronGui() {

}

void OmronGui::SetupLayout() {
    QWidget *widget = new QWidget;
    QVBoxLayout *v_layout = new QVBoxLayout(widget);
    gridLayout_channels = new QGridLayout(widget);
    v_layout->addLayout(gridLayout_channels);

    widget->setLayout(v_layout);
    ui_->scrollArea->setWidget(widget);
}

void OmronGui::SetupDatabase() {
    qDebug() << "OmronGui::SetupDatabase";

    if (!repo_) {
        repo_ = std::unique_ptr<hw_omron::OmronRepo>(new hw_omron::OmronRepo("_prod"));
    }
    if (!repo_->Connect(5)) {
        QMessageBox::critical(this, "Omron", "Connection to database failed");
    }
}

void OmronGui::DisplayOmronChannels(const QMap<QString, hw_omron::Channel> channels) {
    qDebug() << "OmronGui::DisplayOmronChannels";
    const int NFIELDSPERCHANNEL = 12; // n items + spacer
    const int max_nrows = 31;
    const int ncols = static_cast<int>(channels.size()) / max_nrows + (static_cast<int>(channels.size()) % max_nrows ? 1 : 0);
    qDebug() << " NROWS " << max_nrows << " NCOLS " << ncols;
    QFont header_font("LKLUG", 10, QFont::Bold);

    // Titles
    for (int icol = 0; icol < ncols; ++icol) {
        QLabel *lSystem = new QLabel("System");
        lSystem->setFixedHeight(30);
        gridLayout_channels->addWidget(lSystem,    0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::SYSTEM));
        gridLayout_channels->addWidget(new QLabel("Name"),      0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::NAME));
        gridLayout_channels->addWidget(new QLabel("Node"),      0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::NODE));
        gridLayout_channels->addWidget(new QLabel("Area"),      0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::AREA));
        gridLayout_channels->addWidget(new QLabel("Address"),   0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::ADDRESS));
        gridLayout_channels->addWidget(new QLabel("Bit"),       0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::BIT));
        gridLayout_channels->addWidget(new QLabel("Impuls"),   0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::IMPULS));
        gridLayout_channels->addWidget(new QLabel("Read"),      0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::READ));
        gridLayout_channels->addWidget(new QLabel("Write"),     0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::WRITE));
        gridLayout_channels->addWidget(new QLabel("State"),     0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::STATE));
        gridLayout_channels->addWidget(new QLabel("Ping"),     0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::PING));

        QLabel *lSpacer = new QLabel("");
        lSpacer->setFixedWidth(60);
        gridLayout_channels->addWidget(lSpacer, 0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::PING) + 1);

        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::SYSTEM))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::NAME))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::NODE))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::AREA))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::ADDRESS))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::BIT))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::IMPULS))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::READ))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::WRITE))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::STATE))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::PING))->widget())->setFont(header_font);
    }

    for (int iCh = 0; iCh < static_cast<int>(channels.values().size()); ++iCh) {
        int row = 1 + (iCh % max_nrows);
        int col = NFIELDSPERCHANNEL * (iCh / max_nrows);
        hw_omron::Channel ch = channels.values().at(iCh);
        gridLayout_channels->addWidget(new QLabel(ch.system()), row, col + static_cast<int>(COLUMNS::SYSTEM));
        gridLayout_channels->addWidget(new QLabel(ch.name()),   row, col + static_cast<int>(COLUMNS::NAME));
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.node())), row, col + static_cast<int>(COLUMNS::NODE));
        gridLayout_channels->addWidget(new QLabel(FromFinsMemoryArea(ch.fins_memeory_area())), row, col + static_cast<int>(COLUMNS::AREA));
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.address())), row, col + static_cast<int>(COLUMNS::ADDRESS));
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.bit())), row, col + static_cast<int>(COLUMNS::BIT));
        gridLayout_channels->addWidget(new QLabel(ch.impuls() ? QString("T") : QString("F")), row, col + static_cast<int>(COLUMNS::IMPULS));

        QPushButton *bReadValue = new QPushButton("R");
        bReadValue->setFixedWidth(40);
        gridLayout_channels->addWidget(bReadValue, row, col + static_cast<int>(COLUMNS::READ));

        QPushButton *bWriteValue = new QPushButton("W");
        bWriteValue->setFixedWidth(40);
        gridLayout_channels->addWidget(bWriteValue, row, col + static_cast<int>(COLUMNS::WRITE));

        QPushButton *lValue = new QPushButton("0");
        lValue->setFixedWidth(40);
        gridLayout_channels->addWidget(lValue, row, col + static_cast<int>(COLUMNS::STATE));
        QObject::connect(lValue, &QPushButton::clicked, this, [lValue]() {
            lValue->setText(lValue->text() == QString("0") ? QString("1") : QString("0")); });

        QPushButton *bPing = new QPushButton("P");
        bPing->setStyleSheet(QString("QPushButton {background: qlineargradient(x1: 0.5, y1: 0.5 x2: 0.5, y2: 1, stop: 0 #3b4045, stop: 0.5 #31363b)}"));
        bPing->setFixedWidth(40);
        gridLayout_channels->addWidget(bPing, row, col + static_cast<int>(COLUMNS::PING));
        QTimer *pingTimer = new QTimer(bPing);
        pingTimer->setInterval(1000);
        pingTimer->setSingleShot(false);
        QObject::connect(pingTimer, &QTimer::timeout, this, [this, ch]() { ReadChannel(ch.name()); });
        QObject::connect(bPing, &QPushButton::clicked, this, [bPing, pingTimer]() {
            if (bPing->styleSheet() != QString("QPushButton {background: #3daee9}")) {
                bPing->setStyleSheet(QString("QPushButton {background: #3daee9}"));
                pingTimer->start();
            } else {
                bPing->setStyleSheet(QString("QPushButton {background: qlineargradient(x1: 0.5, y1: 0.5 x2: 0.5, y2: 1, stop: 0 #3b4045, stop: 0.5 #31363b)}"));
                pingTimer->stop();
            }});

        QObject::connect(bReadValue, &QPushButton::clicked, this, [this, ch]() { ReadChannel(ch.name()); });
        QObject::connect(bWriteValue, &QPushButton::clicked, this, [this, lValue, ch]() { WriteChannel(ch.name(), lValue); });
        QObject::connect(&omron_, qOverload<uint8_t, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA, uint16_t, uint8_t, bool>
                         (&medicyc::cyclotroncontrolsystem::hardware::omron::Omron::SIGNAL_BitRead), this, [this, lValue, ch](uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, bool content) {
            if (node == ch.node() && memory_area == ch.fins_memeory_area() && address == ch.address() && bit == ch.bit()) {
                lValue->setText(QString::number(content));
                FlashBackground(lValue);
            };
        });

    }
}

void OmronGui::FlashBackground(QPushButton *state) {
    state->setStyleSheet("QPushButton {background: #3daee9}");
    QTimer::singleShot(40, this, [state]() { state->setStyleSheet("QPushButton {background: solid black"); } );
}

void OmronGui::WriteChannel(QString name, QPushButton* value) {
    try {
        qDebug() << "OmronGui::WriteChannel " << name << " Value " << value->text();
        omron_.WriteChannel(name, value->text().toInt());
    } catch (std::exception& exc) {
        QMessageBox::warning(this, "Read channel error", QString::fromStdString(exc.what()));
    }
}

void OmronGui::ReadChannel(QString name) {
    try {
        qDebug() << "OmronGui::ReadChannel " << name;
        omron_.ReadChannel(name);
    } catch (std::exception& exc) {
        QMessageBox::warning(this, "Read channel error", QString::fromStdString(exc.what()));
    }
}

void OmronGui::ReadBit() {
    try {
         const uint8_t node = static_cast<uint8_t>(ui_->lineEdit_read_bit_node->text().toInt());
         const hw_omron::FINS_MEMORY_AREA area_code = static_cast<hw_omron::FINS_MEMORY_AREA>(ui_->comboBox_read_bit_area_code->currentData().toInt());
         const uint16_t address = ui_->lineEdit_read_bit_address->text().toInt();
         const uint8_t bit = ui_->lineEdit_read_bit_number->text().toInt();
         omron_.ReadBit(node, area_code, address, bit);
    } catch (std::exception& exc) {
        QMessageBox::warning(this, "Read bit error", QString::fromStdString(exc.what()));
    }
}

void OmronGui::ReadWord() {
    try {
         const uint8_t node = static_cast<uint8_t>(ui_->lineEdit_read_word_node->text().toInt());
         const hw_omron::FINS_MEMORY_AREA area_code = static_cast<hw_omron::FINS_MEMORY_AREA>(ui_->comboBox_read_word_area_code->currentData().toInt());
         const uint16_t address = ui_->lineEdit_read_word_address->text().toInt();
         omron_.ReadWord(node, area_code, address);
    } catch (std::exception& exc) {
        QMessageBox::warning(this, "Read word error", QString::fromStdString(exc.what()));
    }
}

void OmronGui::WriteBit() {
    try {
        const uint8_t node = static_cast<uint8_t>(ui_->lineEdit_write_bit_node->text().toInt());
         const hw_omron::FINS_MEMORY_AREA area_code = static_cast<hw_omron::FINS_MEMORY_AREA>(ui_->comboBox_write_bit_area_code->currentData().toInt());
         const uint16_t address = ui_->lineEdit_write_bit_address->text().toInt();
         const uint8_t bit = ui_->lineEdit_write_bit_number->text().toInt();
         const uint16_t content = ui_->lineEdit_write_bit_content->text().toInt();
         omron_.WriteBit(node, area_code, address, bit, content);
    } catch (std::exception& exc) {
        QMessageBox::warning(this, "Write bit error", QString::fromStdString(exc.what()));
    }
}

void OmronGui::WriteWord() {
    try {
         const uint8_t node = static_cast<uint8_t>(ui_->lineEdit_write_word_node->text().toInt());
         const hw_omron::FINS_MEMORY_AREA area_code = static_cast<hw_omron::FINS_MEMORY_AREA>(ui_->comboBox_write_word_area_code->currentData().toInt());
         const uint16_t address = ui_->lineEdit_write_word_address->text().toInt();
         const uint16_t content = ui_->lineEdit_write_word_content->text().toInt();
         omron_.WriteWord(node, area_code, address, content);
    } catch (std::exception& exc) {
        QMessageBox::warning(this, "Write word error", QString::fromStdString(exc.what()));
    }
}

void OmronGui::DisplayStateMachineMessage(QString message) {
    (void)message;
//    if (ui_->radioButton_statemachine->isChecked()) ui_->plainTextEdit_io->appendPlainText(message);
}

void OmronGui::DisplayIOError(QString message) {
    (void)message;
//    if (ui_->radioButton_ioerror) ui_->plainTextEdit_io->appendPlainText(QString("IO ERROR: ") + message);
}

void OmronGui::DisplayFINSHeader(hw_omron::FINSHeader header) {
    ui_->label_icf->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::ICF), 2, 16, QLatin1Char('0')));
    ui_->label_rsv->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::RSV), 2, 16, QLatin1Char('0')));
    ui_->label_gct->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::GCT), 2, 16, QLatin1Char('0')));
    ui_->label_dna->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::DNA), 2, 16, QLatin1Char('0')));
    ui_->label_da1->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::DA1), 2, 16, QLatin1Char('0')));
    ui_->label_da2->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::DA2), 2, 16, QLatin1Char('0')));
    ui_->label_sna->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::SNA), 2, 16, QLatin1Char('0')));
    ui_->label_sa1->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::SA1), 2, 16, QLatin1Char('0')));
    ui_->label_sa2->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::SA2), 2, 16, QLatin1Char('0')));
    ui_->label_sid->setText(QString("%1").arg(header.GetFrameField(hw_omron::FINSHeader::FINS_HEADER_FIELD::SID), 2, 16, QLatin1Char('0')));
}

void OmronGui::DisplayWordRead(uint8_t node, hw_omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content) {
    if (node == static_cast<uint8_t>(ui_->lineEdit_read_word_node->text().toInt()) &&
            FromFinsMemoryArea(memory_area) == ui_->comboBox_read_word_area_code->currentText() &&
            address == ui_->lineEdit_read_word_address->text().toInt()) {
        ui_->label_read_word_content->setText(QString::number(content));
    }
}

void OmronGui::DisplayBitRead(uint8_t node, hw_omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, bool content) {
    if (node == static_cast<uint8_t>(ui_->lineEdit_read_bit_node->text().toInt()) &&
            FromFinsMemoryArea(memory_area) == ui_->comboBox_read_bit_area_code->currentText() &&
            address == ui_->lineEdit_read_bit_address->text().toInt() &&
            bit == ui_->lineEdit_read_bit_number->text().toInt()) {
        ui_->label_read_bit_content->setText(QString::number(content));
    }
}

void OmronGui::DisplayWordWritten(uint8_t node, hw_omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content) {
    (void)node;
    (void)memory_area;
    (void)content;
    (void)address;
}

void OmronGui::DisplayBitWritten(uint8_t node, hw_omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, bool content) {
    (void)node;
    (void)memory_area;
    (void)content;
    (void)address;
    (void)bit;
    (void)content;
}

void OmronGui::FillComboBoxes() {
    ui_->comboBox_read_word_area_code->addItem(QString("DM"), static_cast<int>(hw_omron::FINS_MEMORY_AREA::DM));
    ui_->comboBox_read_word_area_code->addItem(QString("CIO"), static_cast<int>(hw_omron::FINS_MEMORY_AREA::CIO));
    ui_->comboBox_read_bit_area_code->addItem(QString("DM"), static_cast<int>(hw_omron::FINS_MEMORY_AREA::DM));
    ui_->comboBox_read_bit_area_code->addItem(QString("CIO"), static_cast<int>(hw_omron::FINS_MEMORY_AREA::CIO));
    ui_->comboBox_write_word_area_code->addItem(QString("DM"), static_cast<int>(hw_omron::FINS_MEMORY_AREA::DM));
    ui_->comboBox_write_word_area_code->addItem(QString("CIO"), static_cast<int>(hw_omron::FINS_MEMORY_AREA::CIO));
    ui_->comboBox_write_bit_area_code->addItem(QString("DM"), static_cast<int>(hw_omron::FINS_MEMORY_AREA::DM));
    ui_->comboBox_write_bit_area_code->addItem(QString("CIO"), static_cast<int>(hw_omron::FINS_MEMORY_AREA::CIO));
}

QString OmronGui::FrameStyleSheet(QString name, QString image) const {
    QString str ("QFrame#" + name + " {"
                 "background-image: url(:/Images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    return str;
}

void OmronGui::ShowPing() {
    ui_->frame_ping->setStyleSheet(FrameStyleSheet("frame_ping", "circle-blue-20x20"));
    QTimer::singleShot(500, this, [&]() { ui_->frame_ping->setStyleSheet("QFrame {}"); });
}

void OmronGui::ShowConnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "connected_90x90.png"));
}

void OmronGui::ShowDisconnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "disconnected_90x90.png"));
}


}
