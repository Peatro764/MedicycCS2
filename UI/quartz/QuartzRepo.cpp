#include "QuartzRepo.h"

#include <QSettings>
#include <QSqlError>
#include <QDebug>
#include <QRectF>

#include "Parameters.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

QuartzRepo::QuartzRepo(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix) {
    qDebug() << "DB name " << db_name_;
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    QString hostName = settings.value(db_name_ + "/host", "unknown").toString();
    QString databaseName = settings.value(db_name_ + "/name", "unknown").toString();
    int port = settings.value(db_name_ + "/port", "0").toInt();
    QString userName = settings.value(db_name_ + "/user", "unknown").toString();
    QString password = settings.value(db_name_ + "/password", "unknown").toString();
    int timeout = settings.value(db_name_ + "/timeout", 3000).toInt();

    qDebug() << "QuartzRepo::Repo Host " << hostName
             << " database " << databaseName
             << " port " << port
             << " username " << userName
             << " password " << password;

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL7", db_name_);
    db.setHostName(hostName);
    db.setPort(port);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(timeout));
}

QuartzRepo::~QuartzRepo() {
    QSqlDatabase::removeDatabase(db_name_);
}

void QuartzRepo::GetBeamAxesCalibration(QString quartz, double &x_min, double &x_max, double &y_min, double &y_max) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "QuartzRepo::GetBeamAxesCalibration Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("SELECT x_min, x_max, y_min, y_max FROM BeamAxesCalibration where quartz = :quartz");
    query.bindValue(":quartz", quartz);

    if (!query.exec()) {
        qWarning() << "QuartzRepo::GetBeamAxesCalibration Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for beam axes calibration failed");
    }

    if (!query.next()) {
        qWarning() << "QuartzRepo::GetBeamAxesCalibration No data found";
        throw std::runtime_error("Db query returned no data");
    }

    x_min = query.value(0).toDouble();
    x_max = query.value(1).toDouble();
    y_min = query.value(2).toDouble();
    y_max = query.value(3).toDouble();
}

void QuartzRepo::SetBeamAxesCalibration(QString quartz, double x_min, double x_max, double y_min, double y_max) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "QuartzRepo::SetBeamAxesCalibration Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("UPDATE BeamAxesCalibration SET x_min = :x_min, x_max = :x_max, y_min = :y_min, y_max = :y_max "
                  "WHERE quartz = :quartz");
    query.bindValue(":x_min", x_min);
    query.bindValue(":x_max", x_max);
    query.bindValue(":y_min", y_min);
    query.bindValue(":y_max", y_max);
    query.bindValue(":quartz", quartz);

    if (!query.exec()) {
        qWarning() << "QuartzRepo::SetBeamAxesCalibration Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for beam axes calibration failed");
    }
}

void QuartzRepo::CreateBeamImageSet(QString name, QString comments) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "QuartzRepo::CreateBeamImageSet Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO BeamImageSet(name, timestamp, comments)"
                  " VALUES(:name, :timestamp, :comments)");
    query.bindValue(":name", name);
    query.bindValue(":timestamp", QDateTime::currentDateTime());
    query.bindValue(":comments", comments);

    if (!query.exec()) {
        qWarning() << "QuartzRepo::CreateBeamImageSet Query failed: " << query.lastQuery() << " " << query.lastError();
        throw std::runtime_error("Db query to create beamimageset failed");
    }
}

void QuartzRepo::SaveBeamImageData(QString beam_image_set, QString quartz, QVector<double> data, int ncol,
                                   double xmin, double xmax, double ymin, double ymax, QRectF zoom) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "QuartzRepo::SaveBeamImageData Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO BeamImage(quartz, beam_image_set, "
                  "x_min, x_max, y_min, y_max, zoom_x, zoom_y, zoom_width, zoom_height, data, ncol)"
                  " VALUES(:quartz, :beam_image_set, :xmin, :xmax, :ymin, :ymax, :zoom_x, :zoom_y, :zoom_width, :zoom_height, :data, :ncol)");
    query.bindValue(":quartz", quartz);
    query.bindValue(":beam_image_set", beam_image_set);
    query.bindValue(":xmin", xmin);
    query.bindValue(":xmax", xmax);
    query.bindValue(":ymin", ymin);
    query.bindValue(":ymax", ymax);
    query.bindValue(":zoom_x", zoom.x());
    query.bindValue(":zoom_y", zoom.y());
    query.bindValue(":zoom_width", zoom.width());
    query.bindValue(":zoom_height", zoom.height());
    query.bindValue(":data", EncodeDoubleArray(data));
    query.bindValue(":ncol", ncol);

    if (!query.exec()) {
        qWarning() << "QuartzRepo::SaveBeamImageData Query failed: " << query.lastQuery() << " error " << query.lastError();
        throw std::runtime_error("Db query to save beam image data failed");
    }
}


QMap<QDateTime, QString> QuartzRepo::GetBeamImageSets() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "QuartzRepo::GetBeamImageSets Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    QString query_string("SELECT timestamp, name FROM BeamImageSet");
    query.prepare(query_string);

    if (!query.exec()) {
        qWarning() << "QuartzRepo::GetBeamImageSets Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for beamimagesets failed");
    }

    QMap<QDateTime, QString> image_sets;
    while (query.next()) {
        image_sets[query.value(0).toDateTime()] = query.value(1).toString();
    }
    return image_sets;
}

QStringList QuartzRepo::GetBeamImageNames(QString image_set) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "QuartzRepo::GetBeamImageNames Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("SELECT quartz FROM BeamImage WHERE beam_image_set = :image_set");
    query.bindValue(":image_set", image_set);

    if (!query.exec()) {
        qWarning() << "QuartzRepo::GetBeamImageNames Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for beam images failed");
    }

    QStringList images;
    while (query.next()) {
        images.append(query.value(0).toString());
    }
    return images;
}

void QuartzRepo::GetBeamImageData(QString beam_image_set, QString quartz, QVector<double>& data, int &ncol,
                              double& xmin, double& xmax, double& ymin, double& ymax, QRectF& zoom) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "QuartzRepo::GetBeamImageData Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("SELECT x_min, x_max, y_min, y_max, zoom_x, zoom_y, zoom_width, zoom_height, data, ncol FROM BeamImage WHERE"
                  " beam_image_set = :beam_image_set AND quartz = :quartz");
    query.bindValue(":beam_image_set", beam_image_set);
    query.bindValue(":quartz", quartz);

    if (!query.exec()) {
        qWarning() << "QuartzRepo::GetImageData Query failed: " << query.lastQuery() << " " << query.lastError();
        throw std::runtime_error("Db query for image data failed");
    }

    if (!query.next()) {
        qWarning() << "QuartzRepo::GetImageData No data found";
        throw std::runtime_error("Db query returned no data");
    }

    xmin = query.value(0).toDouble();
    xmax = query.value(1).toDouble();
    ymin = query.value(2).toDouble();
    ymax = query.value(3).toDouble();
    zoom = QRectF(query.value(4).toDouble(), query.value(5).toDouble(), query.value(6).toDouble(), query.value(7).toDouble());
    data = DecodeDoubleArray(query.value(8).toString());
    ncol = query.value(9).toInt();
    qDebug() << "QuartzRepo::GetBeamImageData xmin " << xmin << " xmax " << xmax << " data " << data.size() << " ncol " << ncol;
}

QVector<double> QuartzRepo::DecodeDoubleArray(QString array) const {
    QString arrayWithoutBraces(array.remove("{").remove("}"));
    QStringList doubleStringList(arrayWithoutBraces.split(",", Qt::SkipEmptyParts));
    QVector<double> doubleVector;
    for (QString& val : doubleStringList) {
        doubleVector.push_back(val.toDouble());
    }
    return doubleVector;
}

QString QuartzRepo::EncodeDoubleArray(QVector<double> values) {
    QString encArray("{");
    for (auto val : values) {
        encArray == "{" ? encArray.append(QString::number(val, 'f', 6)) : encArray.append("," + QString::number(val, 'f', 6));
    }
    encArray.append("}");
    return encArray;
}


}
