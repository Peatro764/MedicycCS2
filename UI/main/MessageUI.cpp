#include "MessageUI.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QSound>
#include <QDateTimeEdit>
#include <QHeaderView>

#include "Utils.h"
#include "DBus.h"
#include "Style.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

MessageUI::MessageUI(QWidget *parent) :
    QWidget(parent),
    messagelogger_interface_(medicyc::cyclotron::MessageLoggerInterface("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()))
{
    ConfigureTableWidget();
    QVBoxLayout* v_layout = new QVBoxLayout(this);
    v_layout->addLayout(CreateTopWindow());
    v_layout->addLayout(CreateMessageWindow());
    v_layout->addSpacing(15);
    v_layout->addLayout(CreateDateSelector());
}

void MessageUI::ConfigureTableWidget() {
    table_widget_.setRowCount(MAX_MESSAGES_);
    table_widget_.setColumnCount(4);
    QStringList headers { "Date", "Type", "Système", "Message" };
    table_widget_.setColumnWidth(static_cast<int>(COLUMNS::DATE), 170);
    table_widget_.setColumnWidth(static_cast<int>(COLUMNS::TYPE), 100);
    table_widget_.setColumnWidth(static_cast<int>(COLUMNS::SYSTEM), 220);
    table_widget_.horizontalHeader()->setStretchLastSection(true);
    table_widget_.verticalHeader()->hide();

    table_widget_.verticalHeader()->setDefaultSectionSize(12);
    table_widget_.setHorizontalHeaderLabels(headers);
    table_widget_.setFont(QFont("Arial", 12));
}

QRadioButton* MessageUI::GetRadioButton(QString text) {
    QRadioButton *r = new QRadioButton(text);
    r->setStyleSheet("QRadioButton::indicator{width: 30px;height: 30px; }\n"
                     "QRadioButton { background-color: transparent; font-size: 16px; }");
    r->setAutoExclusive(false);
    return r;
}

QHBoxLayout* MessageUI::CreateTypeSelector() {
    QRadioButton* r_error = GetRadioButton("Erreur");
    QRadioButton* r_warning = GetRadioButton("Alerte");
    QRadioButton* r_info = GetRadioButton("Info");
    QRadioButton* r_debug = GetRadioButton("Debug");
    QRadioButton* r_update = GetRadioButton("Mise à jour auto.");
    r_error->setChecked(show_error_);
    r_warning->setChecked(show_warning_);
    r_info->setChecked(show_info_);
    r_debug->setChecked(show_debug_);
    r_update->setChecked(update_auto_);
    QObject::connect(r_error, &QRadioButton::clicked, this, [&, this](bool checked) { show_error_ = checked; });
    QObject::connect(r_warning, &QRadioButton::clicked, this, [&, this](bool checked) { show_warning_ = checked; });
    QObject::connect(r_info, &QRadioButton::clicked, this, [&, this](bool checked) { show_info_ = checked; });
    QObject::connect(r_debug, &QRadioButton::clicked, this, [&, this](bool checked) { show_debug_ = checked; });
    QObject::connect(r_update, &QRadioButton::clicked, this, [&, this](bool checked) { update_auto_ = checked; });

    QHBoxLayout* h_layout = new QHBoxLayout;
    h_layout->addWidget(r_error);
    h_layout->addWidget(r_warning);
    h_layout->addWidget(r_info);
    h_layout->addWidget(r_debug);
    h_layout->addWidget(r_update);
    return h_layout;
}

QHBoxLayout* MessageUI::CreateDateSelector() {
    // date time selection
    int height(38);
    QDateTimeEdit* date_from = new QDateTimeEdit;
    date_from->setDisplayFormat("yyyy.MM.dd");
    date_from->setCalendarPopup(true);
    date_from->setDate(QDate::currentDate().addDays(-1));
    date_from->setFixedHeight(height);
    QDateTimeEdit* date_to = new QDateTimeEdit;
    date_to->setDisplayFormat("yyyy.MM.dd");
    date_to->setCalendarPopup(true);
    date_to->setDate(QDate::currentDate());
    date_to->setFixedHeight(height);
    QLabel  *l_from = new QLabel("De");
    l_from->setAttribute(Qt::WA_TranslucentBackground);
    l_from->setSizePolicy(QSizePolicy::Minimum , QSizePolicy::Minimum);
    QLabel  *l_to = new QLabel("A");
    l_to->setAttribute(Qt::WA_TranslucentBackground);
    QPushButton *p_get = new QPushButton("Chercher");
    p_get->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QPushButton *p_clear = new QPushButton("Effacer");
    p_clear->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QHBoxLayout* h_layout = new QHBoxLayout;
    h_layout->addWidget(l_from);
    h_layout->addWidget(date_from);
    h_layout->addWidget(l_to);
    h_layout->addWidget(date_to);
    h_layout->addWidget(p_get);
    h_layout->addWidget(p_clear);
    QObject::connect(p_get, &QPushButton::clicked, this, [&, date_from, date_to]() { GetData(date_from->date(), date_to->date()); });
    QObject::connect(p_clear, &QPushButton::clicked, this, [&]() {
            table_widget_.clearContents();
            active_row_ = 0;
    });
    return h_layout;
}

QHBoxLayout* MessageUI::CreateTopWindow() {
    // title
    QLabel *l_title = shared::utils::GetLabel("Messages", shared::FONT::TITLE);
    l_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    l_title->setMinimumWidth(300);

    QHBoxLayout *h_layout = new QHBoxLayout();
    h_layout->addWidget(l_title);
    h_layout->addLayout(CreateTypeSelector());
    return h_layout;
}

QVBoxLayout* MessageUI::CreateMessageWindow() {

    QObject::connect(&messagelogger_interface_, &medicyc::cyclotron::MessageLoggerInterface::SIGNAL_Error, this, [this](qlonglong datetime, const QString &system, const QString &message) {
        if (!blocked_systems_.contains(system) && show_error_) {
            DisplayMessage("Erreur", datetime, system, message, Qt::red);
            emit SIGNAL_PlayErrorSound();
        }});
    QObject::connect(&messagelogger_interface_, &medicyc::cyclotron::MessageLoggerInterface::SIGNAL_Warning, this, [this](qlonglong datetime, const QString &system, const QString &message) {
        if (!blocked_systems_.contains(system) && show_warning_) {
            DisplayMessage("Alerte", datetime, system, message, Qt::yellow);
        }});
    QObject::connect(&messagelogger_interface_, &medicyc::cyclotron::MessageLoggerInterface::SIGNAL_Info, this, [this](qlonglong datetime, const QString &system, const QString &message) {
        if (!blocked_systems_.contains(system) && show_info_) {
            DisplayMessage("Info", datetime, system, message, Qt::white);
        }});
    QObject::connect(&messagelogger_interface_, &medicyc::cyclotron::MessageLoggerInterface::SIGNAL_Debug, this, [this](qlonglong datetime, const QString &system, const QString &message) {
        if (!blocked_systems_.contains(system) && show_debug_) {
            DisplayMessage("Debug", datetime, system, message, Qt::green);
        }});
    QObject::connect(&messagelogger_interface_, &medicyc::cyclotron::MessageLoggerInterface::SIGNAL_RetrievedMessages, this, [this](qlonglong datetime, const QString &system, const QString& type, const QString &message) {
        if (type == "ERROR" && show_error_) {
            DisplayMessage("Erreur", datetime, system, message, Qt::red);
        } else if (type == "WARNING" && show_warning_) {
            DisplayMessage("Alerte", datetime, system, message, Qt::yellow);
        } else if (type == "INFO" && show_info_) {
            DisplayMessage("Info", datetime, system, message, Qt::white);
        } else if (type == "DEBUG" && show_debug_) {
            DisplayMessage("Debug", datetime, system, message, Qt::green);
        } else {
            DisplayMessage("UNKNOWN", datetime, system, message, Qt::white);
        }
    });
    // Put everything on a layout
    QVBoxLayout* v_layout = new QVBoxLayout(this);
    v_layout->addWidget(&table_widget_);
    return v_layout;
}

void MessageUI::DisplayMessage(QString type, qlonglong datetime, QString system, QString message, QColor color) {
    QString formatted_type = type.leftJustified(10, QChar(' '), true);
    QString formatted_system = system.leftJustified(30, QChar(' '), true);
    QString formatted_message = QDateTime::fromMSecsSinceEpoch(datetime).toString("yyyy-MM-dd hh:mm:ss") + "          " + formatted_type + "     " +  formatted_system + "     " + message;

    auto date_item = new QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(datetime).toString("yyyy-MM-dd hh:mm:ss"));
    auto type_item = new QTableWidgetItem(type);
    auto system_item = new QTableWidgetItem(system);
    auto message_item = new QTableWidgetItem(message);
    date_item->setForeground(QBrush(color));
    type_item->setForeground(QBrush(color));
    system_item->setForeground(QBrush(color));
    message_item->setForeground(QBrush(color));

    if (active_row_ >= (MAX_MESSAGES_ - 1)) {
        table_widget_.removeRow(0);
        table_widget_.insertRow(table_widget_.rowCount());
    }
    table_widget_.setItem(active_row_, static_cast<int>(COLUMNS::DATE), date_item);
    table_widget_.setItem(active_row_, static_cast<int>(COLUMNS::TYPE), type_item);
    table_widget_.setItem(active_row_, static_cast<int>(COLUMNS::SYSTEM), system_item);
    table_widget_.setItem(active_row_, static_cast<int>(COLUMNS::MESSAGE), message_item);
    if (update_auto_) table_widget_.scrollToItem(date_item);
    active_row_ = std::min(active_row_ + 1, MAX_MESSAGES_ - 1);
};

void MessageUI::GetData(QDate from, QDate to) {
    if (from < to) {
        messagelogger_interface_.GetMessages(from.startOfDay().toMSecsSinceEpoch(), to.endOfDay().toMSecsSinceEpoch());
    } else {
        ErrorMessage("La date de début doit être antérieure à la date de fin");
    }
}

MessageUI::~MessageUI() {
}

void MessageUI::ErrorMessage(QString message) {
    messagelogger_interface_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("MainUI"), message);
}

void MessageUI::WarningMessage(QString message) {
    messagelogger_interface_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("MainUI"), message);
}

void MessageUI::InfoMessage(QString message) {
    messagelogger_interface_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("MainUI"), message);
}

void MessageUI::DebugMessage(QString message) {
    messagelogger_interface_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("MainUI"), message);
}

}
