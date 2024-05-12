#ifndef MEDICYCCS2_ADC_REPOSITORY_H
#define MEDICYCCS2_ADC_REPOSITORY_H

#include "ADC_global.h"

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>

#include "ADC.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

class ADC_EXPORT ADCRepo : public QObject {
  Q_OBJECT
public:
    ADCRepo(QString name);
    ~ADCRepo();
    bool Connect(int timeout);
    void Disconnect();
    bool IsConnected();

    QStringList GetADCChannelNames() const;
    Channel GetADCChannel(QString name) const;
private:
    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;
};

}

#endif
