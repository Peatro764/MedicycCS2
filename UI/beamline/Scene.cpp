#include "Scene.h"

#include <QDebug>
#include <QGraphicsRectItem>
#include <QSound>
#include <QPlainTextEdit>
#include <QPushButton>

#include "DBus.h"
#include "Parameters.h"
#include "qgraphicsproxywidget.h"
#include "qscrollbar.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

Scene::Scene(QObject * parent) : QGraphicsScene(parent),
    settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
              QSettings::IniFormat),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    beamline_state_interface_("medicyc.cyclotron.middlelayer.beamlinestate", "/BeamLineState", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{

    qDebug() << "Scene::Scene";

    send_log_messages_to_server_ = settings_.value("ui/beamstate/send_log_to_server", "True").toBool();

    AddBeamLine();
    AddDipoles();
    AddQuadrupoles();
    AddCFs();
    AddQZs();
    AddDT();
    AddRooms();
    AddCyclotron();
    AddMessagebox();
    AddClock();

    SetupBeamLineStateInterface();
    SetupHardwareActions();
    QBrush brush;
    QColor color(Qt::yellow);
    color.setAlpha(255);
    brush.setColor(color);
    setBackgroundBrush(brush);

    // Set gradient background
     QLinearGradient gradient(0, 0, width(), height());
     gradient.setSpread(QGradient::RepeatSpread);
     gradient.setColorAt(0, Qt::black);
     gradient.setColorAt(1, QColor("cornflowerblue"));
     gradient.setSpread(QGradient::PadSpread);
     setBackgroundBrush(gradient);
}

Scene::~Scene()
{
    qDebug() << "Scene::~Scene";
}

QString Scene::PATH2STR(Constants::BEAMLINE_PATHS path) {
    if (path == Constants::BEAMLINE_PATHS::STRAIGHT) return QString("DROITE");
    if (path == Constants::BEAMLINE_PATHS::RD_M2) return QString("RD_M2");
    if (path == Constants::BEAMLINE_PATHS::RD_M4) return QString("RD_M4");
    if (path == Constants::BEAMLINE_PATHS::CLINICAL) return QString("CLINIQUE");
    return QString("INCONNUE");
}

void Scene::SetupBeamLineStateInterface() {
    if (!beamline_state_interface_.isValid()) {
        qWarning() << "Scene::SetupBeamLineStateInterface beamline state interface not valid";
    }
    QObject::connect(&beamline_state_interface_, &medicyc::cyclotron::middlelayer::BeamLineStateInterface::SIGNAL_CFState, this, &Scene::SetCFState);
    QObject::connect(&beamline_state_interface_, &medicyc::cyclotron::middlelayer::BeamLineStateInterface::SIGNAL_QZState, this, &Scene::SetQZState);
    QObject::connect(&beamline_state_interface_, &medicyc::cyclotron::middlelayer::BeamLineStateInterface::SIGNAL_DTState, this, &Scene::SetDTState);
    QObject::connect(&beamline_state_interface_, &medicyc::cyclotron::middlelayer::BeamLineStateInterface::SIGNAL_GuillotineState, this, &Scene::SetBeamState);
    QObject::connect(&beamline_state_interface_, &medicyc::cyclotron::middlelayer::BeamLineStateInterface::SIGNAL_DipoleState, this, &Scene::SetDipoleState);
    // Diaphragme currently not used
}

void Scene::SetupHardwareActions() {
    // DT triggers
    QObject::connect(this, &Scene::SIGNAL_DTStateChange, this, [&](bool insert) {
        if (insert) WarningMessage(QString("DT inséré")); else InfoMessage(QString("DT extrait"));
    });
    QObject::connect(this, &Scene::SIGNAL_DTStateChange, this, &Scene::CheckDTState);

    // Dipole triggers
    QObject::connect(this, &Scene::SIGNAL_DipoleStateChanged, this, &Scene::CheckActivePath);
    QObject::connect(this, &Scene::SIGNAL_PathChanged, this, [&](Constants::BEAMLINE_PATHS path) {
        WarningMessage("Chemin de faisceau : " + PATH2STR(path));
    });
    QObject::connect(this, &Scene::SIGNAL_PathChanged, this, &Scene::CheckPathDistance);
    QObject::connect(this, &Scene::SIGNAL_PathChanged, this, &Scene::DrawBeam);
    QObject::connect(this, &Scene::SIGNAL_PathChanged, this, &Scene::DrawRoom);
    QObject::connect(this, &Scene::SIGNAL_PathChanged, this, &Scene::CheckDTState);

    // CF triggers
    QObject::connect(this, &Scene::SIGNAL_CFStateChange, this, [&](QString name, bool insert) {
        if (insert) InfoMessage(name + QString(" insérée")); else WarningMessage(name + QString(" extraite"));
    });
    QObject::connect(this, &Scene::SIGNAL_CFStateChange, this, &Scene::CheckPathDistance);
    QObject::connect(this, &Scene::SIGNAL_CFStateChange, this, &Scene::DrawBeam);
    QObject::connect(this, &Scene::SIGNAL_CFStateChange, this, &Scene::DrawRoom);
    QObject::connect(this, &Scene::SIGNAL_CFStateChange, this, &Scene::CheckDTState);

    // QZ triggers
    QObject::connect(this, &Scene::SIGNAL_QZStateChange, this, [&](QString name, bool insert) {
        if (insert) WarningMessage(name + QString(" inséré")); else InfoMessage(name + QString(" extrait"));
    });
    QObject::connect(this, &Scene::SIGNAL_QZStateChange, this, &Scene::CheckPathDistance);
    QObject::connect(this, &Scene::SIGNAL_QZStateChange, this, &Scene::DrawBeam);
    QObject::connect(this, &Scene::SIGNAL_QZStateChange, this, &Scene::DrawRoom);
    QObject::connect(this, &Scene::SIGNAL_QZStateChange, this, &Scene::CheckDTState);

    // Guillotine triggers
    QObject::connect(this, &Scene::SIGNAL_BeamStateChanged, this, [&](bool on) {
        if (on) WarningMessage(QString("Guillotine ouverte")); else InfoMessage(QString("Guillotine fermée"));
    });
    QObject::connect(this, &Scene::SIGNAL_BeamStateChanged, this, &Scene::DrawBeam);
    QObject::connect(this, &Scene::SIGNAL_BeamStateChanged, this, &Scene::DrawRoom);
    QObject::connect(this, &Scene::SIGNAL_BeamStateChanged, cyclotron_, &Cyclotron::SetBeam);
    QObject::connect(this, &Scene::SIGNAL_BeamStateChanged, this, &Scene::CheckDTState);
}

// DERIVE CURRENT STATE (active path, path open/closed, beam on/off)

void Scene::CheckActivePath() {
    auto new_path = GetActivePath();
    bool path_changed = (new_path != active_path_);
    active_path_ = new_path;
    if (path_changed) emit SIGNAL_PathChanged(active_path_);
}

Constants::BEAMLINE_PATHS Scene::GetActivePath() const {
    if (dipoles_["M1"]->IsOff()) return Constants::BEAMLINE_PATHS::STRAIGHT;
    if (dipoles_["M2"]->IsOn()) return Constants::BEAMLINE_PATHS::RD_M2;
    if (dipoles_["M4"]->IsOff()) return Constants::BEAMLINE_PATHS::RD_M4;
    return Constants::BEAMLINE_PATHS::CLINICAL;
}

void Scene::CheckPathDistance() {
    bool path_open = true;
    double path_distance = beamline_->GetBeamPath(active_path_)->TotalLength();
    for (QString cf : cfs_.keys()) {
        if (cfs_[cf]->Inserted() && cfs_[cf]->OnSegment(active_path_)) {
            path_open = false;
           path_distance = std::min(path_distance, cfs_[cf]->Distance());
        }
    }
//    for (QString qz : qzs_.keys()) {
//        if (qzs_[qz]->Inserted() && qzs_[qz]->OnSegment(active_path_) && qz != QString("QZ4")) {
//            path_open = false;
//            path_distance = std::min(path_distance, qzs_[qz]->Distance());
//        }
//    }
    path_open_ = path_open;
    path_distance_ = path_distance;
    if (path_open_) {
        emit SIGNAL_PathClosed(path_distance);
    } else {
        emit SIGNAL_PathOpen();
    }
}

void Scene::CheckDTState() {
    if (rooms_[active_path_]->BeamDelivered() && dt_->Inserted()) {
        WarningMessage(QString("DT inséré et faisceau livré à ") + PATH2STR(active_path_));
        QSound::play(":/sounds/DoigtTournant.wav");
    }
}

void Scene::DrawBeam() {
    beamline_->SetBeam(active_path_, path_distance_, beam_on_);
}

void Scene::DrawRoom() {
    for (auto key : rooms_.keys()) {
        qDebug() << QDateTime::currentDateTime() << " Scene::DrawRoom " << rooms_[key]->Name() << " False";
        if (key != active_path_) rooms_[key]->SetBeam(false, false);
    }
    qDebug() << QDateTime::currentDateTime() << " Scene::DrawRoom " << rooms_[active_path_]->Name()
             << " Path open " << path_open_ << " beam on " << beam_on_;
    rooms_[active_path_]->SetBeam(true, path_open_ && beam_on_);
}

void Scene::AddClock() {
    clock_.setText("This is a clock");
    clock_.setFixedWidth(350);
    clock_.setFixedHeight(45);
    clock_.setStyleSheet("background-color: transparent;");

    QFont font("Times", 25, QFont::Bold);
    clock_.setFont(font);
    QTimer *tUpdateClock = new QTimer(this);
    tUpdateClock->setInterval(1000);
    connect(tUpdateClock, &QTimer::timeout, [&]() {
       QString time = QDateTime::currentDateTime().toString("dd MMMM yyyy hh:mm:ss");
       clock_.setText(time);
    } );
    tUpdateClock->start();
    QGraphicsProxyWidget * item = this->addWidget(&clock_);
    item->setPos(-195,-100);
}

// SET COMPONENT STATES BASED ON HARDWARE SIGNALS

void Scene::SetDipoleState(QString name, bool on) {
    if (dipoles_.contains(name)) {
        bool state_changed = dipoles_[name]->SetState(on);
        if (state_changed) emit SIGNAL_DipoleStateChanged(name, on);
    } else {
        qDebug() << "Scene::TurnOnDipole No such dipole: " << name;
    }
}

void Scene::SetCFState(QString id, bool insert) {
    if (cfs_.contains(id)) {
        bool state_changed = cfs_[id]->SetState(insert);
        if (state_changed) emit SIGNAL_CFStateChange(id, insert);
    } else {
        qDebug() << "Scene::SetCFState No such CF: " << id;
    }
}

void Scene::SetQZState(QString id, bool insert) {
    if (qzs_.contains(id)) {
        bool state_changed = qzs_[id]->SetState(insert);
        if (state_changed) emit SIGNAL_QZStateChange(id, insert);
    } else {
        qDebug() << "Scene::SetQZState No such QZ: " << id;
    }
}

void Scene::SetDTState(bool insert) {
    bool state_changed = dt_->SetState(insert);
    if (state_changed) emit SIGNAL_DTStateChange(insert);
}

void Scene::SetBeamState(bool on) {
    bool state_changed = (on != beam_on_);
    beam_on_ = on;
    if (state_changed) {
        emit SIGNAL_BeamStateChanged(on);
    }
}

// ADD COMPONENTS OF MEDICYC

void Scene::AddBeamLine() {
    beamline_ = new BeamLine;
    addItem(beamline_);
    beamline_->moveBy(center_point_.x(), center_point_.y());
}

void Scene::AddRooms() {
    auto CreateRoom = [this] (QString name, double size, Constants::BEAMLINE_PATHS path) -> Room* {
        BeamPath* beam_path = beamline_->GetBeamPath(path);
        double distance = beam_path->TotalLength();
        BeamItem item(distance, 0.0, std::vector<Constants::BEAMLINE_PATHS>{ path });
        Room *room = new Room(name, size, item);
        QObject::connect(room, &Room::SIGNAL_BeamDelivered, this, [&, path]() { WarningMessage("Faisceau livré à " + PATH2STR(path)); });
        room->moveBy(center_point_.x(), center_point_.y());
        QPoint p =  beam_path->Coordinate(distance - 1);
        room->moveBy(p.x(), p.y());
        addItem(room);
        return room;
    };
    rooms_[Constants::BEAMLINE_PATHS::STRAIGHT] = CreateRoom("", 0, Constants::BEAMLINE_PATHS::STRAIGHT);
    rooms_[Constants::BEAMLINE_PATHS::CLINICAL] = CreateRoom("Salle Clinique", Constants::ROOM_DIAMETER, Constants::BEAMLINE_PATHS::CLINICAL);
    QRectF rectClinic = rooms_[Constants::BEAMLINE_PATHS::CLINICAL]->boundingRect();
    rooms_[Constants::BEAMLINE_PATHS::CLINICAL]->moveBy(-rectClinic.width()/2, 4);
    rooms_[Constants::BEAMLINE_PATHS::RD_M4] = CreateRoom("Salle R&D M4", Constants::ROOM_DIAMETER, Constants::BEAMLINE_PATHS::RD_M4);
    QRectF rectM4 = rooms_[Constants::BEAMLINE_PATHS::RD_M4]->boundingRect();
    rooms_[Constants::BEAMLINE_PATHS::RD_M4]->moveBy(3, -rectM4.height() / 2);
    rooms_[Constants::BEAMLINE_PATHS::RD_M2] = CreateRoom("Salle R&D M2", Constants::ROOM_DIAMETER, Constants::BEAMLINE_PATHS::RD_M2);
    //QRectF rectM2 = rooms_["Salle R&D M2"]->boundingRect();
    rooms_[Constants::BEAMLINE_PATHS::RD_M2]->moveBy(-8, -8);
}

void Scene::AddCyclotron() {
    auto path_clinical = beamline_->GetBeamPath(Constants::BEAMLINE_PATHS::CLINICAL);
    std::vector<double> lengths_clinical = path_clinical->SegmentLengths();
    std::vector<double> angles_clinical = path_clinical->SegmentAngles();

    cyclotron_ = new Cyclotron("Cyclotron", Constants::CYCLOTRON_DIAMETER, angles_clinical.at(0), 6.6*Constants::PIPE_OUTER_RADIUS/Constants::CYCLOTRON_DIAMETER/2.0);
    cyclotron_->moveBy(center_point_.x(), center_point_.y());
    cyclotron_->moveBy(5 + -Constants::CYCLOTRON_DIAMETER/2*(1 + std::cos(angles_clinical.at(0))), -5 -Constants::CYCLOTRON_DIAMETER/2*(1 - std::sin(angles_clinical.at(0))));
    addItem(cyclotron_);
}

void Scene::AddMessagebox() {
    textEditLogger_.setMinimumHeight(600);
    textEditLogger_.setMaximumHeight(600);
    textEditLogger_.setMinimumWidth(600);
    textEditLogger_.setMaximumWidth(600);
    textEditLogger_.setMaximumBlockCount(20);

    textEditLogger_.setAttribute(Qt::WA_NoSystemBackground);
    textEditLogger_.setAttribute(Qt::WA_TranslucentBackground);
    textEditLogger_.setAttribute(Qt::WA_PaintOnScreen);
    textEditLogger_.setReadOnly(true);
    textEditLogger_.setStyleSheet("background-color: transparent;");
    textEditLogger_.setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    textEditLogger_.setFont(QFont("Arial", 15));

    QGraphicsProxyWidget * item = this->addWidget(&textEditLogger_);
    item->setPos(200,350);
    textEditLogger_.verticalScrollBar()->setValue(textEditLogger_.verticalScrollBar()->maximum());
    textEditLogger_.verticalScrollBar()->hide();
}

void Scene::DisplayMessage(QDateTime time, QString message, QColor color) {
    qDebug() << "Scene::DisplayMessage " << message;
    if (message.size() > MAX_MESSAGE_SIZE) {
        message = message.left(MAX_MESSAGE_SIZE);
        message.append("...");
    }
    QString formatted_message = time.toString("yyyy-mm-dd hh:mm:ss") + QString("     ") + message;
    QTextCharFormat tf;
    tf = textEditLogger_.currentCharFormat();
    tf.setTextOutline(QPen(color));
    textEditLogger_.setCurrentCharFormat(tf);
    textEditLogger_.appendPlainText(formatted_message);

    // Temporary (see comment Message.h)
    messages_.push_back(Message(formatted_message, color));
    if (static_cast<int>(messages_.size()) > MAX_SAVED_MESSAGES) messages_.erase(messages_.begin());
    update();
};

void Scene::AddDipoles() {
    auto CreateDipole = [this] (QString name, double distance, double span_angle, std::vector<Constants::BEAMLINE_PATHS> paths) -> Dipole* {
        qDebug() << "Adding dipole " << name << " distance " << distance << " " << span_angle;
        BeamPath* path = beamline_->GetBeamPath(paths.front());
        BeamItem item(distance, path->SegmentAngle(distance-1), paths);
        Dipole* dipole = new Dipole(name, span_angle, item);
        dipole->moveBy(center_point_.x(), center_point_.y());
        // move to correct position on the beam pipe
        QPoint p =  path->Coordinate(distance);
        dipole->moveBy(p.x(), p.y());
        // rotate to match rotation of beam pipe (requires degrees and not radians)
        dipole->setRotation(-dipole->Angle() * 180.0 / 3.1415);
        addItem(dipole);
        return dipole;
    };

    auto path_clinical = beamline_->GetBeamPath(Constants::BEAMLINE_PATHS::CLINICAL);
    int ns_clinical = path_clinical->NSegments();
    std::vector<double> segment_lengths_clinical = path_clinical->SegmentLengths();
    std::vector<double> segment_angles_clinical = path_clinical->SegmentAngles();

    // M1
    dipoles_["M1"] = CreateDipole("M1", segment_lengths_clinical.at(0), std::abs(segment_angles_clinical.at(0) - segment_angles_clinical.at(1)),
    std::vector<Constants::BEAMLINE_PATHS> {
                                      Constants::BEAMLINE_PATHS::STRAIGHT,
                                      Constants::BEAMLINE_PATHS::CLINICAL,
                                      Constants::BEAMLINE_PATHS::RD_M2,
                                      Constants::BEAMLINE_PATHS::RD_M4 });
    // M2
    auto path_m2 = beamline_->GetBeamPath(Constants::BEAMLINE_PATHS::RD_M2);
    int ns_m2 = path_m2->NSegments();
    std::vector<double> segment_lengths_m2 = path_m2->SegmentLengths();
    std::vector<double> segment_angles_m2 = path_m2->SegmentAngles();
    double distance_m2 = std::reduce(segment_lengths_m2.begin(), segment_lengths_m2.end() - 1);
    dipoles_["M2"] = CreateDipole("M2", distance_m2, std::abs(segment_angles_m2.at(ns_m2 - 2) - segment_angles_m2.at(ns_m2 - 1)),
    std::vector<Constants::BEAMLINE_PATHS> {
                                      Constants::BEAMLINE_PATHS::CLINICAL,
                                      Constants::BEAMLINE_PATHS::RD_M2,
                                      Constants::BEAMLINE_PATHS::RD_M4 });

    // M4
    double distance_m4 = std::reduce(segment_lengths_clinical.begin(), segment_lengths_clinical.end() - 1);
    dipoles_["M4"] = CreateDipole("M4", distance_m4, std::abs(segment_angles_clinical.at(ns_clinical - 2) - segment_angles_clinical.at(ns_clinical - 1)),
    std::vector<Constants::BEAMLINE_PATHS> {
                                      Constants::BEAMLINE_PATHS::CLINICAL,
                                      Constants::BEAMLINE_PATHS::RD_M4 });
}

void Scene::AddQuadrupoles() {
    auto CreateQuad = [this] (QString name, double distance, std::vector<Constants::BEAMLINE_PATHS> paths) -> Quadrupole* {
        qDebug() << "Adding quadrupole " << name << " distance " << distance;
        BeamPath* path = beamline_->GetBeamPath(paths.front());
        BeamItem item(distance, path->SegmentAngle(distance), paths);
        Quadrupole* quad = new Quadrupole(name, item);
        quad->moveBy(center_point_.x(), center_point_.y());
        // move to correct position on the beam pipe
        QPoint p =  path->Coordinate(distance);
        quad->moveBy(p.x(), p.y());
        // move from centered in the beam pipe to above it
        double delta = -quad->boundingRect().height() / 2;
        quad->moveBy(delta*sin(quad->Angle()), delta*cos(quad->Angle()));
        // rotate to match rotation of beam pipe (requires degrees and not radians)
        quad->setRotation(-quad->Angle() * 180.0 / 3.1415);
        addItem(quad);
        return quad;
    };

    quadrupoles_["Q1"] = CreateQuad("Q1", Constants::SIZE_MULTIPLIER * 320, std::vector<Constants::BEAMLINE_PATHS> {
                                           Constants::BEAMLINE_PATHS::STRAIGHT,
                                           Constants::BEAMLINE_PATHS::CLINICAL,
                                           Constants::BEAMLINE_PATHS::RD_M2,
                                           Constants::BEAMLINE_PATHS::RD_M4 });
    quadrupoles_["Q2"] = CreateQuad("Q2", Constants::SIZE_MULTIPLIER * 470, std::vector<Constants::BEAMLINE_PATHS> {
                                           Constants::BEAMLINE_PATHS::STRAIGHT,
                                           Constants::BEAMLINE_PATHS::CLINICAL,
                                           Constants::BEAMLINE_PATHS::RD_M2,
                                           Constants::BEAMLINE_PATHS::RD_M4 });
    quadrupoles_["Q3"] = CreateQuad("Q3", Constants::SIZE_MULTIPLIER * 1500, std::vector<Constants::BEAMLINE_PATHS> {
                                           Constants::BEAMLINE_PATHS::CLINICAL,
                                           Constants::BEAMLINE_PATHS::RD_M2,
                                           Constants::BEAMLINE_PATHS::RD_M4 });
    quadrupoles_["Q4"] = CreateQuad("Q4", Constants::SIZE_MULTIPLIER * 2000, std::vector<Constants::BEAMLINE_PATHS> {
                                           Constants::BEAMLINE_PATHS::CLINICAL,
                                           Constants::BEAMLINE_PATHS::RD_M2,
                                           Constants::BEAMLINE_PATHS::RD_M4 });
    quadrupoles_["Q5"] = CreateQuad("Q5", Constants::SIZE_MULTIPLIER * 2140, std::vector<Constants::BEAMLINE_PATHS> {
                                           Constants::BEAMLINE_PATHS::CLINICAL,
                                           Constants::BEAMLINE_PATHS::RD_M2,
                                           Constants::BEAMLINE_PATHS::RD_M4 });
    quadrupoles_["Q8"] = CreateQuad("Q8", Constants::SIZE_MULTIPLIER * 3060, std::vector<Constants::BEAMLINE_PATHS> {
                                           Constants::BEAMLINE_PATHS::CLINICAL,
                                           Constants::BEAMLINE_PATHS::RD_M4 });
    quadrupoles_["Q9"] = CreateQuad("Q9", Constants::SIZE_MULTIPLIER * 3200, std::vector<Constants::BEAMLINE_PATHS> {
                                           Constants::BEAMLINE_PATHS::CLINICAL,
                                           Constants::BEAMLINE_PATHS::RD_M4 });
}

void Scene::AddCFs() {
    auto CreateCF = [this] (QString name, double distance,  std::vector<Constants::BEAMLINE_PATHS> paths) -> CF* {
        qDebug() << "Adding CF " << name;
        BeamPath* path = beamline_->GetBeamPath(paths.front());
        BeamItem item(distance, path->SegmentAngle(distance), paths);
        CF* cf = new CF(name, item);
        cf->moveBy(center_point_.x(), center_point_.y());
        // move to correct position on the beam pipe
        QPoint p =  path->Coordinate(distance);
        cf->moveBy(p.x(), p.y());
        // move from centered in the beam pipe to above it
        double delta = -cf->boundingRect().height() + Constants::PIPE_INNER_RADIUS - 1;
        cf->moveBy(delta*sin(cf->Angle()), delta*cos(cf->Angle()));
        // rotate to match rotation of beam pip
        cf->setRotation(-cf->Angle() * 180.0 / 3.1415);
        addItem(cf);
        return cf;
    };

    cfs_["CF1"] = CreateCF("CF1", Constants::SIZE_MULTIPLIER * 80, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::STRAIGHT,
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    cfs_["CF2"] = CreateCF("CF2", Constants::SIZE_MULTIPLIER * 910, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::STRAIGHT,
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    cfs_["CF4"] = CreateCF("CF4", Constants::SIZE_MULTIPLIER * 1820, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    cfs_["CF5"] = CreateCF("CF5", Constants::SIZE_MULTIPLIER * 2480, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    cfs_["CF8"] = CreateCF("CF8", Constants::SIZE_MULTIPLIER * 3470, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    cfs_["CF9"] = CreateCF("CF9", Constants::SIZE_MULTIPLIER * 4220, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL });
    cfs_["CF9-2"] = CreateCF("CF9-2", Constants::SIZE_MULTIPLIER * 4700, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL });

    cfs_["CF10"] = CreateCF("CF10",Constants::SIZE_MULTIPLIER *3975, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::RD_M4 });
}

void Scene::AddDT() {
    qDebug() << "Adding DT";
    const double distance = Constants::SIZE_MULTIPLIER * 650;
    BeamPath* path = beamline_->GetBeamPath(Constants::BEAMLINE_PATHS::STRAIGHT);
    BeamItem item(distance, path->SegmentAngle(distance), std::vector<Constants::BEAMLINE_PATHS> {
                      Constants::BEAMLINE_PATHS::STRAIGHT,
                      Constants::BEAMLINE_PATHS::CLINICAL,
                      Constants::BEAMLINE_PATHS::RD_M2,
                      Constants::BEAMLINE_PATHS::RD_M4 });
    dt_ = new DT(item);
    dt_->moveBy(center_point_.x(), center_point_.y());
    // move to correct position on the beam pipe
    QPoint p =  path->Coordinate(distance);
    dt_->moveBy(p.x(), p.y());
    // move from centered in the beam pipe to above it
    double delta = -dt_->boundingRect().height() + Constants::PIPE_INNER_RADIUS - 1;
    dt_->moveBy(delta*sin(dt_->Angle()), delta*cos(dt_->Angle()));
    // rotate to match rotation of beam pip
    dt_->setRotation(-dt_->Angle() * 180.0 / 3.1415);
    addItem(dt_);
}

void Scene::AddQZs() {
    auto CreateQZ = [this] (QString name, double distance,  std::vector<Constants::BEAMLINE_PATHS> paths) -> QZ* {
        qDebug() << "Adding  QZ " << name;
        BeamPath* path = beamline_->GetBeamPath(paths.front());
        BeamItem item(distance, path->SegmentAngle(distance), paths);
        QZ* qz = new QZ(name, item);
        qz->moveBy(center_point_.x(), center_point_.y());
        // move to correct position on the beam pipe
        QPoint p =  path->Coordinate(distance);
        qz->moveBy(p.x(), p.y());
        // move from centered in the beam pipe to above it
        double delta = -qz->boundingRect().height() + Constants::PIPE_INNER_RADIUS - 1;
        qz->moveBy(delta*sin(qz->Angle()), delta*cos(qz->Angle()));
        // rotate to match rotation of beam pip
        qz->setRotation(-qz->Angle() * 180.0 / 3.1415);
        addItem(qz);
        return qz;
    };
    qzs_["QZ1"] = CreateQZ("QZ1", Constants::SIZE_MULTIPLIER * 180, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::STRAIGHT,
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    qzs_["QZ2"] = CreateQZ("QZ2", Constants::SIZE_MULTIPLIER * 810, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::STRAIGHT,
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    qzs_["QZ3"] = CreateQZ("QZ3", Constants::SIZE_MULTIPLIER * 1250, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    qzs_["QZ4"] = CreateQZ("QZ4", Constants::SIZE_MULTIPLIER * 1720, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    qzs_["QZ5"] = CreateQZ("QZ5", Constants::SIZE_MULTIPLIER * 2380, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    qzs_["QZ6"] = CreateQZ("QZ6", Constants::SIZE_MULTIPLIER * 2700, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M2,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    qzs_["QZ8"] = CreateQZ("QZ8", Constants::SIZE_MULTIPLIER * 3370, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL,
                               Constants::BEAMLINE_PATHS::RD_M4 });

    qzs_["QZ9"] = CreateQZ("QZ9", Constants::SIZE_MULTIPLIER *4120, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::CLINICAL });

    qzs_["QZ10"] = CreateQZ("QZ10", Constants::SIZE_MULTIPLIER * 3865, std::vector<Constants::BEAMLINE_PATHS> {
                               Constants::BEAMLINE_PATHS::RD_M4 });

}

void Scene::ErrorMessage(QString message) {
    DisplayMessage(QDateTime::currentDateTime(), message, Qt::red);
    if (send_log_messages_to_server_) logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), SUB_SYSTEM, message);
}

void Scene::WarningMessage(QString message) {
    DisplayMessage(QDateTime::currentDateTime(), message, Qt::yellow);
    if (send_log_messages_to_server_) logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), SUB_SYSTEM, message);
}

void Scene::InfoMessage(QString message) {
    qDebug() << "Scene::InfoMessage " << message;
    DisplayMessage(QDateTime::currentDateTime(), message, Qt::white);
    if (send_log_messages_to_server_) logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), SUB_SYSTEM, message);
}

} // ns
