#ifndef SCENE_H
#define SCENE_H

#include <QSettings>
#include <QStandardPaths>
#include <QGraphicsScene>
#include <QMap>
#include <QPlainTextEdit>
#include <QLabel>

#include "Cyclotron.h"
#include "Room.h"
#include "CF.h"
#include "QZ.h"
#include "DT.h"
#include "Dipole.h"
#include "Quadrupole.h"
#include "BeamLine.h"
#include "Message.h"

#include "messagelogger_interface.h"
#include "beamlinestate_interface.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Scene(QObject * parent = nullptr);
    ~Scene();

    // Temporary (see comment Message.h)
     std::vector<Message> GetMessages() const { return messages_; }


public slots: // to make private once testing interface is done
    void SetBeamState(bool on);
    void SetDTState(bool insert);
    void SetCFState(QString id, bool insert);
    void SetQZState(QString id, bool insert);
    void SetDipoleState(QString name, bool on);

private slots:
    void DrawBeam();
    void DrawRoom();
    void AddClock();
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DisplayMessage(QDateTime time, QString message, QColor color);

signals:
    void SIGNAL_CFStateChange(QString id, bool insert);
    void SIGNAL_QZStateChange(QString id, bool insert);
    void SIGNAL_DTStateChange(bool insert);
    void SIGNAL_DipoleStateChanged(QString id, bool on);
    void SIGNAL_BeamStateChanged(bool on);
    void SIGNAL_PathChanged(Constants::BEAMLINE_PATHS new_path);
    void SIGNAL_PathOpen();
    void SIGNAL_PathClosed(double distance);

private:
    void SetupBeamLineStateInterface();
    void SetupHardwareActions();

    void AddBeamLine();
    void AddDipoles();
    void AddCFs();
    void AddQZs();
    void AddDT();
    void AddQuadrupoles();
    void AddRooms();
    void AddCyclotron();
    void AddMessagebox();

    void CheckPathDistance();
    void CheckActivePath();
    Constants::BEAMLINE_PATHS GetActivePath() const;
    void CheckDTState();

    QString PATH2STR(Constants::BEAMLINE_PATHS path);

    QSettings settings_;
    bool send_log_messages_to_server_ = false;

    QPlainTextEdit textEditLogger_;
    QLabel clock_;

    // State variables
    bool beam_on_ = false;
    Constants::BEAMLINE_PATHS active_path_ = Constants::BEAMLINE_PATHS::STRAIGHT;
    bool path_open_ = false;
    double path_distance_ = 0;

    // Beamline components
    QMap<Constants::BEAMLINE_PATHS, Room*> rooms_;
    QMap<QString, CF*> cfs_;
    QMap<QString, QZ*> qzs_;
    DT* dt_;
    QMap<QString, Dipole*> dipoles_;
    QMap<QString, Quadrupole*> quadrupoles_;
    Cyclotron *cyclotron_;
    BeamLine *beamline_;
    QPoint center_point_ = QPoint(-120, 400);

    // Hardware interfaces
    enum class INTERFACE { LOGGER, OMRON, IOR, M1, M2, M4 };
    QMap<INTERFACE, bool> status_dbus_;
    medicyc::cyclotron::MessageLoggerInterface logger_;
    medicyc::cyclotron::middlelayer::BeamLineStateInterface beamline_state_interface_;

    const QString SUB_SYSTEM = "FAISCEAU";
    const int MAX_MESSAGE_SIZE = 35;
    // Temporary (see comment Message.h)
    const int MAX_SAVED_MESSAGES = 1000;
    std::vector<Message> messages_;
};

}

#endif
