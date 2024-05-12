#ifndef REPLYPARSER_H
#define REPLYPARSER_H

#include <QObject>
#include <QString>

#include "Config.h"
#include "Measurement.h"
#include "Error.h"
#include "StateReply.h"
#include "AddressRegister.h"
#include "StateCommand.h"
#include "Command.h"
#include "NSingleConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class ReplyParser : public QObject
{
    Q_OBJECT
public:
    ReplyParser(NSingleConfig config);
    ~ReplyParser();

public slots:
    void Parse(QByteArray data);

signals:
    void SIGNAL_ConfigUpdated(medicyc::cyclotroncontrolsystem::hardware::nsingle::Config config);
    void SIGNAL_ErrorUpdated(medicyc::cyclotroncontrolsystem::hardware::nsingle::Error error); // nsingle command execution error
    void SIGNAL_StateUpdated(medicyc::cyclotroncontrolsystem::hardware::nsingle::StateReply state_reply); // partially duplicated in signal on/off/error below
    void SIGNAL_On();
    void SIGNAL_Off();
    void SIGNAL_Error(); // nsingle error ("defaut")
    void SIGNAL_Ok();
    void SIGNAL_Local();
    void SIGNAL_Remote();

    void SIGNAL_AddressValue(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address);
    void SIGNAL_AddressSetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address);

    void SIGNAL_Channel1Value(double physical_value, bool polarity);
    void SIGNAL_Channel1Value(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
    void SIGNAL_Channel1SetPoint(double physical_value, bool polarity);
    void SIGNAL_Channel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
    void SIGNAL_Channel2Value(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);

    void SIGNAL_ReplyReceived();
    void SIGNAL_ParseError(QString message);

private:
    void ProcessAddressSetPoint(QString body);
    void ProcessAddressValue(QString body);

    void ProcessChannel1SetPoint(QString body);
    void ProcessChannel1Value(QString body);
    void ProcessChannel2Value(QString body);

    void ProcessConfig(QString body);
    void ProcessStateChange(QString body);
    void ProcessError(QString body);

    const QByteArray cmd_footer_ = "*\r\n";
    NSingleConfig config_;
};

} // namespace

#endif
