#include "ReplyParser.h"

#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

ReplyParser::ReplyParser() {}

ReplyParser::ReplyParser(QString separator, QString footer)
    : footer_(footer), separator_(separator) {}

void ReplyParser::Parse(const QByteArray& line) {
    Parse(QString(line));
}

void ReplyParser::Parse(QString line) {
    qDebug() << "ReplyParser::Parse " << line;
    const QString footer(line.mid(line.size() - footer_.size()));
    if (footer != footer_) {
        qDebug() << "ReplyParser::Parse Corrupt reply footer: " << footer << " Complete line: " << line;
        emit(ReplyError("Reply footer is not correct: " + footer));
        return;
    }

    line = line.left(line.size() - footer.size());

    if (line.right(1) == separator_) {
        line = line.left(line.size() - 1);
    }

    QStringList items(line.split(separator_, Qt::SkipEmptyParts));
    if (items.empty()) {
        qDebug() << "ReplyParser::Parse Corrupt reply: " << line;
        emit ReplyError("Reply is empty: " + line);
        return;
    }
    const QString header(items.at(0));
    const QStringList body(items.mid(1, items.size()));

    if (header == "L0I=" || header == ">0I=") {
        ProcessInstantenousMeasurementReply(body);
    } else if (header == "L1I=" || header == ">1I=" || header == "1I=") {
        ProcessIntegratedMeasurement1Reply(body);
    } else if (header == "L2I=") {
        ProcessIntegratedMeasurement2Reply(body);
    } else if (header == "LP0=" || header == ">P0=" || header == "P0=") {
        ProcessModeParameterReply(body);
    } else if (header == "LP1=" || header == ">P1=" || header == "P1=") {
        ProcessNumericParameterReply(body);
    } else if (header == "ETA=") {
        ProcessThresholdStatusReply(body);
    } else if (header == "BU1=" || header == ">U1=" || header == "U1=") {
        qDebug() << "ReplyParser::Parse ReadBuffer header: " << header;
        ProcessBufferIntegratedMeasurement1(body);
    } else if (header == "BU2=") {
        ProcessBufferIntegratedMeasurement2(body);
    } else if (header == "DAT=") {
        ProcessDate(body);
    } else if (header == "> ") {
        emit(CommandConfirmed());
    } else if (header.startsWith("ERR")) {
        ProcessErrorReply(header.mid(4));
    } else {
        qDebug() << "ReplyParser::Parse Unknown reply. Header " << header << " full line: " << line;
        emit(ReplyError("Unknown reply: " + line));
        return;
    }
}

void ReplyParser::ProcessInstantenousMeasurementReply(QStringList body) {
   InstantenousMeasurement m(ParseMeasurement(body));
//   emit(CommandReply());  // These are emitted automatically
   emit(InstantenousMeasurementChanged(m));
}

void ReplyParser::ProcessIntegratedMeasurement1Reply(QStringList body) {
    IntegratedMeasurement m(ParseMeasurement(body));
    emit(CommandReply());
    emit(IntegratedMeasurement1Changed(m));
}

void ReplyParser::ProcessIntegratedMeasurement2Reply(QStringList body) {
    IntegratedMeasurement m(ParseMeasurement(body));
    emit(CommandReply());
    emit(IntegratedMeasurement2Changed(m));
}

void ReplyParser::ProcessBufferIntegratedMeasurement1(QStringList body) {
    QStringList body_act = body.mid(0, body.size() / 2);
    QStringList body_pre = body.mid(body.size() / 2, body.size() / 2);
    IntegratedMeasurement m_act(ParseMeasurement(body_act));
    IntegratedMeasurement m_pre(ParseMeasurement(body_pre));
    emit(CommandReply());
    emit(BufferIntegratedMeasurement1Changed(IntegratedMeasurementBuffer(m_pre, m_act)));
}

void ReplyParser::ProcessBufferIntegratedMeasurement2(QStringList body) {
    QStringList body_act = body.mid(0, body.size() / 2);
    QStringList body_pre = body.mid(body.size() / 2, body.size() / 2);
    IntegratedMeasurement m_act(ParseMeasurement(body_act));
    IntegratedMeasurement m_pre(ParseMeasurement(body_pre));
    emit(CommandReply());
    emit(BufferIntegratedMeasurement2Changed(IntegratedMeasurementBuffer(m_pre, m_act)));
}

Measurement ReplyParser::ParseMeasurement(QStringList body) {
    QDateTime timestamp;
    double raw_value(0.0);
    double conv_value(0.0);

    switch (body.size()) {
    case 1:
        conv_value = body.at(0).toDouble();
        break;
    case 2:
        raw_value = body.at(0).toDouble();
        conv_value = body.at(1).toDouble();
        break;
    case 7:
        timestamp = DateFromString(body.mid(0, 6));
        conv_value = body.at(6).toDouble();
        break;
    case 8:
        timestamp = DateFromString(body.mid(0, 6));
        raw_value = body.at(6).toDouble();
        conv_value = body.at(7).toDouble();
        break;
    default:
        qDebug() << "ReplyParser::ParseMeasurement Reply corrupt: " << body.join(":");
        emit(ReplyError(QString("Measurement reply is corrupt")));
        break;
    }

    return Measurement(timestamp, raw_value, conv_value);
}

void ReplyParser::ProcessModeParameterReply(QStringList body) {
    if (body.size() != 9) {
        emit(ReplyError("Mode parameter reply has wrong format: " + body.join(separator_)));
        return;
    }
    emit(CommandReply());
    emit(ModeFunctionnementChanged(static_cast<ModeFunctionnement>(body.at(0).toInt())));
    emit(PreAmpliTypeChanged(static_cast<PreAmpliType>(body.at(1).toInt())));
    emit(PreAmpliGammeChanged(static_cast<PreAmpliGamme>(body.at(2).toInt())));
    emit(EmitInstantenousMeasurementChanged(body.at(4).toInt()));
    emit(EmitIntegrated1MeasurementChanged(body.at(5).toInt()));
    emit(EmitIntegrated2MeasurementChanged(body.at(6).toInt()));
    emit(IncludeDateInMeasurementChanged(body.at(7).toInt()));
    emit(IncludeConversedValueInMeasurementChanged(body.at(8).toInt()));
}

void ReplyParser::ProcessNumericParameterReply(QStringList body) {
    if (body.size() != 12) {
        emit(ReplyError("Numeric parameter reply has wrong format: " + body.join(separator_)));
        return;
    }

    emit(CommandReply());
    emit(InstantenousMeasurementTimeChanged(static_cast<int>(body.at(0).toDouble())));
    emit(InstantenousMeasurementElementsChanged(static_cast<int>(body.at(1).toDouble())));
    emit(NmbMeasurementsBelowThresholdBeforeAlarmResetChanged(static_cast<int>(body.at(2).toDouble())));
    emit(IntegratedMeasurement1TimeChanged(static_cast<int>(body.at(3).toDouble())));
    emit(IntegratedMeasurement2TimeChanged(static_cast<int>(body.at(4).toDouble())));
    emit(NmbMeasurementsIntegrationTime2Changed(static_cast<int>(body.at(5).toDouble())));
    emit(InstantenousMeasurementConversionCoeffChanged(body.at(6).toDouble()));
    emit(IntegratedMeasurementConversionCoeffChanged(body.at(7).toDouble()));
    emit(IntegratedMeasurementThresholdLevelChanged(body.at(8).toDouble()));
    emit(WarningThreshold1Changed(body.at(9).toDouble()));
    emit(WarningThreshold2Changed(body.at(10).toDouble()));
    emit(WarningThreshold3Changed(body.at(11).toDouble()));
}

void ReplyParser::ProcessDate(QStringList body) {
    if (body.size() != 6) {
        emit(ReplyError("Date reply has wrong format: " + body.join(separator_)));
        return;
    }
    emit(CommandReply());
    emit(DateChanged(DateFromString(body)));
}

void ReplyParser::ProcessThresholdStatusReply(QStringList body) {
    if (body.size() != 1 && body.size() != 7) {
        emit(ReplyError("Threshold status message corrupt: " + body.join(separator_)));
        return;
    }

    bool ok(false);
    int code(0);
    if (body.size() == 1) {
        code = body.at(0).toInt(&ok);
    } else {
        code = body.at(body.size() - 1).toInt(&ok);
    }
    if (ok) {
        emit(CommandReply());
        emit(ThresholdStatusChanged(static_cast<ThresholdStatus>(code)));
    } else {
        emit(ReplyError("Threshold status value corrupt:" + body.join(":")));
    }
}

void ReplyParser::ProcessErrorReply(QString body) {
    bool ok(false);
    const int error_code = body.toInt(&ok);
    if (ok) {
        switch (error_code) {
        case 10:
            emit BaliseError("Configuration lost");
            break;
        case 19:
            emit BaliseError("Unknown command");
            break;
        case 15:
            emit BaliseError("Low power OR watch dog activated");
            break;
        case 20:
            emit BaliseError("Message too long");
            break;
        case 21:
            emit BaliseError("Parameterization not allowed during measurement");
            break;
        case 25:
            emit BaliseError("Parameter out of limits");
            break;
        case 32:
            emit BaliseError("Message IEEE unknown");
            break;
        case 40:
            emit BaliseError("Input buffer saturated");
            break;
        case 42:
            emit BaliseError("Serial port error");
            break;
        case 44:
            emit BaliseError("Previous message not yet treated");
            break;
        case 50:
            emit BaliseError("Command not possible to execute");
            break;
        case 60:
            emit BaliseError("Acces ram secourue en defaut");
            break;
        case 70:
            emit BaliseError("Parameter not valid OR measurement about to start");
            break;
        default:
            emit BaliseError("Unknown error code: " + QString::number(error_code));
            break;
        }
    }
}

QDateTime ReplyParser::DateFromString(const QStringList& body) const {
    QDateTime timestamp = QDateTime::fromString(body.join(" "), QString("yy M d h m s"));
    return timestamp.addYears(100);
}

}

