#ifndef MEDICYCCS2_QUARTZREPO_H
#define MEDICYCCS2_QUARTZREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>
#include <QStandardPaths>

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

class QuartzRepo : public QObject {
  Q_OBJECT
public:
    QuartzRepo(QString db_suffix);
    ~QuartzRepo();
    void GetBeamAxesCalibration(QString quartz, double &x_min, double &x_max, double &y_min, double& y_max);
    void SetBeamAxesCalibration(QString quartz, double x_min, double x_max, double y_min, double y_max);
    void CreateBeamImageSet(QString name, QString comments);
    QMap<QDateTime, QString> GetBeamImageSets() const;
    QStringList GetBeamImageNames(QString image_set) const;
    void SaveBeamImageData(QString beam_image_set, QString quartz, QVector<double> data, int ncol,
                           double xmin, double xmax, double ymin, double ymax, QRectF zoom);
    void GetBeamImageData(QString beam_image_set, QString quartz, QVector<double>& data, int &ncol,
                          double& xmin, double& xmax, double& ymin, double& ymax, QRectF& zoom) const;

private:
    QString EncodeDoubleArray(QVector<double> values);
    QVector<double> DecodeDoubleArray(QString array) const;
    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;
};

}

#endif
