#include "BeamLine.h"

#include <QColor>
#include <QBrush>
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include <QGradient>

#include "Constants.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

BeamLine::BeamLine() : QGraphicsObject() {
    qDebug() << "BeamLine::BeamLine";
    BuildBeamPaths();
    BuildBeamPipe();
    BuildBeamErasers();
    SetBeam(Constants::BEAMLINE_PATHS::STRAIGHT, 0, false);
}

void BeamLine::SetBeam(Constants::BEAMLINE_PATHS segment, double distance, bool beam_on) {
    BuildHypotheticalBeam(segment);
    BuildBeam(segment, beam_on ? distance : 0.0);
    update();
}

BeamPath* BeamLine::GetBeamPath(Constants::BEAMLINE_PATHS path) {
    return beam_paths_[path];
}

QRectF BeamLine::boundingRect() const
{
    return pipe_outer1_.boundingRect()
            .united((pipe_outer2_.boundingRect()))
            .united(pipe_inner1_.boundingRect())
            .united(pipe_inner2_.boundingRect());
}

void BeamLine::BuildHypotheticalBeam(Constants::BEAMLINE_PATHS path) {
    BeamPath* beam_path = beam_paths_[path];
    QPolygon polygon = beam_path->Path(beam_path->TotalLength());
    beam_hypothetical_.clear();
    beam_hypothetical_.addPolygon(polygon);
}

void BeamLine::BuildBeam(Constants::BEAMLINE_PATHS path, double distance) {
    QPolygon polygon = beam_paths_[path]->Path(distance);
    beam_actual_.clear();
    beam_actual_.addPolygon(polygon);
}

void BeamLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidth(14);
    painter->setBrush(Qt::transparent);

    // Hypothetical beam
    pen.setColor(QColor("cornflowerblue"));
    painter->setPen(pen);
    painter->drawPath(beam_hypothetical_);

    // Beam
    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawPath(beam_actual_);

    // Beampipe
    pen.setWidth(2);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setBrush(QColor("#DBE2E9"));
    painter->drawPath(pipe_outer1_);
    painter->drawPath(pipe_inner1_);
    painter->drawPath(pipe_inner2_);
    painter->drawPath(pipe_outer2_);
}


// Builders for beam pipe and beam path

void BeamLine::BuildBeamPaths() {
    QPoint p0(0, 0);
    QPoint p1(Constants::SIZE_MULTIPLIER * (458 + 400), -Constants::SIZE_MULTIPLIER * (458 + 400));
    QPoint p2(1.02*p1);
    QPoint p3(p1 + QPoint(Constants::SIZE_MULTIPLIER*1600, 0)); //1997
    QPoint p4(p3 + QPoint(Constants::SIZE_MULTIPLIER*232, Constants::SIZE_MULTIPLIER*232));
    QPoint p5(p3 + QPoint(Constants::SIZE_MULTIPLIER*922, 0));
    QPoint p6(p5 + QPoint(Constants::SIZE_MULTIPLIER*350, 0));
    QPoint p7(p5 + QPoint(0, Constants::SIZE_MULTIPLIER*(1071 + 400)));

    beam_paths_[Constants::BEAMLINE_PATHS::CLINICAL] = new BeamPath(new BeamPoint(p0,
                                                                    new BeamPoint(p1,
                                                                    new BeamPoint(p5,
                                                                    new BeamPoint(p7, nullptr)))));
    beam_paths_[Constants::BEAMLINE_PATHS::STRAIGHT] = new BeamPath(new BeamPoint(p0,
                                                                    new BeamPoint(p1,
                                                                    new BeamPoint(p2,nullptr))));
    beam_paths_[Constants::BEAMLINE_PATHS::RD_M2] = new BeamPath(new BeamPoint(p0,
                                                                 new BeamPoint(p1,
                                                                 new BeamPoint(p3,
                                                                 new BeamPoint(p4, nullptr)))));
    beam_paths_[Constants::BEAMLINE_PATHS::RD_M4] = new BeamPath(new BeamPoint(p0,
                                                                 new BeamPoint(p1,
                                                                 new BeamPoint(p6,nullptr))));
    qDebug() << "Built straight beam path " << beam_paths_[Constants::BEAMLINE_PATHS::STRAIGHT]->TotalLength();
    qDebug() << "Built clinical beam path " << beam_paths_[Constants::BEAMLINE_PATHS::CLINICAL]->TotalLength();
    qDebug() << "Built rd m2 beam path " << beam_paths_[Constants::BEAMLINE_PATHS::RD_M2]->TotalLength();
    qDebug() << "Built rd m4 beam path " << beam_paths_[Constants::BEAMLINE_PATHS::RD_M4]->TotalLength();
}

QPolygon BeamLine::GetOuter1Pipe() const {
    QPolygon polygon;
    BeamPath* path = beam_paths_[Constants::BEAMLINE_PATHS::RD_M4];
    std::vector<double> angles = path->SegmentAngles();
    std::vector<double> lengths = path->SegmentLengths();

    // outer
    polygon << path->Coordinate(0) +
               QPoint(-Constants::PIPE_OUTER_RADIUS*std::sin(angles.at(0)), -Constants::PIPE_OUTER_RADIUS*std::cos(angles.at(0)));
    polygon << path->Coordinate(lengths.at(0)) +
               QPoint(-Constants::PIPE_OUTER_RADIUS*std::sin(angles.at(0)), -Constants::PIPE_OUTER_RADIUS*std::cos(angles.at(0)));
    polygon << path->Coordinate(lengths.at(0)) +
               QPoint(0, -Constants::PIPE_OUTER_RADIUS);
    polygon << path->Coordinate(lengths.at(0) + lengths.at(1) - 1) +
               QPoint(0, -Constants::PIPE_OUTER_RADIUS);
    // inner
    polygon << path->Coordinate(lengths.at(0) + lengths.at(1) - 1) +
               QPoint(0, -Constants::PIPE_INNER_RADIUS);
    polygon << path->Coordinate(lengths.at(0)) +
               QPoint(0, -Constants::PIPE_INNER_RADIUS);
    polygon << path->Coordinate(lengths.at(0)) +
               QPoint(-Constants::PIPE_INNER_RADIUS*std::sin(angles.at(0)), -Constants::PIPE_INNER_RADIUS*std::cos(angles.at(0)));
    polygon << path->Coordinate(0) +
               QPoint(-Constants::PIPE_INNER_RADIUS*std::sin(angles.at(0)), -Constants::PIPE_INNER_RADIUS*std::cos(angles.at(0)));
    polygon << path->Coordinate(0) +
               QPoint(-Constants::PIPE_OUTER_RADIUS*std::sin(angles.at(0)), -Constants::PIPE_OUTER_RADIUS*std::cos(angles.at(0)));
   return polygon;
}

QPolygon BeamLine::GetOuter2Pipe() const {
    QPolygon polygon;
    BeamPath* path_m4 = beam_paths_[Constants::BEAMLINE_PATHS::RD_M4];
    std::vector<double> angles_m4 = path_m4->SegmentAngles();
    std::vector<double> lengths_m4 = path_m4->SegmentLengths();
    BeamPath* path_clinic = beam_paths_[Constants::BEAMLINE_PATHS::CLINICAL];
    std::vector<double> angles_clinic = path_clinic->SegmentAngles();
    std::vector<double> lengths_clinic = path_clinic->SegmentLengths();

    // Outer
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1) + lengths_clinic.at(2) - 1) +
               QPoint(Constants::PIPE_OUTER_RADIUS, 0);
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1)) +
               QPoint(Constants::PIPE_OUTER_RADIUS, Constants::PIPE_OUTER_RADIUS);
    polygon << path_m4->Coordinate(lengths_m4.at(0) + lengths_m4.at(1) - 1) +
               QPoint(0, Constants::PIPE_OUTER_RADIUS);
    // Inner
    polygon << path_m4->Coordinate(lengths_m4.at(0) + lengths_m4.at(1) - 1) +
               QPoint(0, Constants::PIPE_INNER_RADIUS);
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1)) +
               QPoint(Constants::PIPE_INNER_RADIUS, Constants::PIPE_INNER_RADIUS);
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1) + lengths_clinic.at(2) - 1) +
               QPoint(Constants::PIPE_INNER_RADIUS, 0);
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1) + lengths_clinic.at(2) - 1) +
               QPoint(Constants::PIPE_OUTER_RADIUS, 0);
    return polygon;
}

QPolygon BeamLine::GetInner1Pipe() const {
    QPolygon polygon;
    BeamPath* path = beam_paths_[Constants::BEAMLINE_PATHS::RD_M2];
    std::vector<double> angles = path->SegmentAngles();
    std::vector<double> lengths = path->SegmentLengths();

    // outer
    polygon << path->Coordinate(0) +
               QPoint(Constants::PIPE_OUTER_RADIUS*std::sin(angles.at(0)), Constants::PIPE_OUTER_RADIUS*std::cos(angles.at(0)));
    polygon << path->Coordinate(lengths.at(0) - (Constants::PIPE_OUTER_RADIUS / std::sin(angles.at(0)) - Constants::PIPE_OUTER_RADIUS / std::tan(angles.at(0)))) +
               QPoint(Constants::PIPE_OUTER_RADIUS*std::sin(angles.at(0)), Constants::PIPE_OUTER_RADIUS*std::cos(angles.at(0)));
    polygon << path->Coordinate(lengths.at(0) + lengths.at(1) - (Constants::PIPE_OUTER_RADIUS / std::sin(angles.at(2) - angles.at(1)) - Constants::PIPE_OUTER_RADIUS / std::tan(angles.at(2) - angles.at(1)))) +
               QPoint(Constants::PIPE_OUTER_RADIUS*std::sin(angles.at(2) - angles.at(1)), Constants::PIPE_OUTER_RADIUS*std::cos(angles.at(2) - angles.at(1)));
    polygon << path->Coordinate(lengths.at(0) + lengths.at(1) + lengths.at(2) - 1) +
               QPoint(-Constants::PIPE_OUTER_RADIUS*std::cos(angles.at(1) - angles.at(2)), Constants::PIPE_OUTER_RADIUS*std::sin(angles.at(1) - angles.at(2)));
    // inner
    polygon << path->Coordinate(lengths.at(0) + lengths.at(1) + lengths.at(2) - 1) +
               QPoint(-Constants::PIPE_INNER_RADIUS*std::cos(angles.at(1) - angles.at(2)), Constants::PIPE_INNER_RADIUS*std::sin(angles.at(1) - angles.at(2)));
    polygon << path->Coordinate(lengths.at(0) + lengths.at(1) - (Constants::PIPE_INNER_RADIUS / std::sin(angles.at(2) - angles.at(1)) - Constants::PIPE_INNER_RADIUS / std::tan(angles.at(2) - angles.at(1)))) +
               QPoint(Constants::PIPE_INNER_RADIUS*std::sin(angles.at(2) - angles.at(1)), Constants::PIPE_INNER_RADIUS*std::cos(angles.at(2) - angles.at(1)));
    polygon << path->Coordinate(lengths.at(0) - (Constants::PIPE_INNER_RADIUS / std::sin(angles.at(0)) - Constants::PIPE_INNER_RADIUS / std::tan(angles.at(0)))) +
               QPoint(Constants::PIPE_INNER_RADIUS*std::sin(angles.at(0)), Constants::PIPE_INNER_RADIUS*std::cos(angles.at(0)));
    polygon << path->Coordinate(0) +
               QPoint(Constants::PIPE_INNER_RADIUS*std::sin(angles.at(0)), Constants::PIPE_INNER_RADIUS*std::cos(angles.at(0)));
    polygon << path->Coordinate(0) +
               QPoint(Constants::PIPE_OUTER_RADIUS*std::sin(angles.at(0)), Constants::PIPE_OUTER_RADIUS*std::cos(angles.at(0)));
    return polygon;
}

QPolygon BeamLine::GetInner2Pipe() const {
    QPolygon polygon;
    BeamPath* path_m2 = beam_paths_[Constants::BEAMLINE_PATHS::RD_M2];
    std::vector<double> angles_m2 = path_m2->SegmentAngles();
    std::vector<double> lengths_m2 = path_m2->SegmentLengths();
    BeamPath* path_clinic = beam_paths_[Constants::BEAMLINE_PATHS::CLINICAL];
    std::vector<double> angles_clinic = path_clinic->SegmentAngles();
    std::vector<double> lengths_clinic = path_clinic->SegmentLengths();

    // outer
    polygon << path_m2->Coordinate(lengths_m2.at(0) + lengths_m2.at(1) + lengths_m2.at(2) - 1) +
               QPoint(Constants::PIPE_OUTER_RADIUS*std::cos(angles_m2.at(1) - angles_m2.at(2)), -Constants::PIPE_OUTER_RADIUS*std::sin(angles_m2.at(1) - angles_m2.at(2)));
    polygon << path_m2->Coordinate(lengths_m2.at(0) + lengths_m2.at(1) + lengths_m2.at(2) - 1) +
               QPoint(Constants::PIPE_OUTER_RADIUS*std::cos(angles_m2.at(1) - angles_m2.at(2)), -Constants::PIPE_OUTER_RADIUS*std::sin(angles_m2.at(1) - angles_m2.at(2)));
    polygon << path_m2->Coordinate(lengths_m2.at(0) + lengths_m2.at(1)) +
               QPoint(Constants::PIPE_OUTER_RADIUS * (1 + 1 / std::sin(90.0*3.1415/180.0 - (angles_m2.at(1) - angles_m2.at(2))) / std::tan(angles_m2.at(1) - angles_m2.at(2))),
                      Constants::PIPE_OUTER_RADIUS);
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1)) +
               QPoint(-Constants::PIPE_OUTER_RADIUS, Constants::PIPE_OUTER_RADIUS);
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1) + lengths_clinic.at(2) - 1) +
               QPoint(-Constants::PIPE_OUTER_RADIUS, 0);
    // inner
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1) + lengths_clinic.at(2) - 1) +
               QPoint(-Constants::PIPE_INNER_RADIUS, 0);
    polygon << path_clinic->Coordinate(lengths_clinic.at(0) + lengths_clinic.at(1)) +
               QPoint(-Constants::PIPE_INNER_RADIUS, Constants::PIPE_INNER_RADIUS);
    polygon << path_m2->Coordinate(lengths_m2.at(0) + lengths_m2.at(1)) +
               QPoint(Constants::PIPE_INNER_RADIUS * (1 + 1 / std::sin(90.0*3.1415/180.0 - (angles_m2.at(1) - angles_m2.at(2))) / std::tan(angles_m2.at(1) - angles_m2.at(2))),
                      Constants::PIPE_INNER_RADIUS);
    polygon << path_m2->Coordinate(lengths_m2.at(0) + lengths_m2.at(1) + lengths_m2.at(2) - 1) +
               QPoint(Constants::PIPE_INNER_RADIUS*std::cos(angles_m2.at(1) - angles_m2.at(2)), -Constants::PIPE_INNER_RADIUS*std::sin(angles_m2.at(1) - angles_m2.at(2)));
    polygon << path_m2->Coordinate(lengths_m2.at(0) + lengths_m2.at(1) + lengths_m2.at(2) - 1) +
               QPoint(Constants::PIPE_INNER_RADIUS*std::cos(angles_m2.at(1) - angles_m2.at(2)), -Constants::PIPE_INNER_RADIUS*std::sin(angles_m2.at(1) - angles_m2.at(2)));
    polygon << path_m2->Coordinate(lengths_m2.at(0) + lengths_m2.at(1) + lengths_m2.at(2) - 1) +
               QPoint(Constants::PIPE_OUTER_RADIUS*std::cos(angles_m2.at(1) - angles_m2.at(2)), -Constants::PIPE_OUTER_RADIUS*std::sin(angles_m2.at(1) - angles_m2.at(2)));
    return polygon;
}

void BeamLine::BuildBeamPipe() {
    QPolygon pipe_outer_polygon = GetOuter1Pipe();
    pipe_outer1_.addPolygon(pipe_outer_polygon);
    QPolygon pipe_outer2_polygon = GetOuter2Pipe();
    pipe_outer2_.addPolygon(pipe_outer2_polygon);
    QPolygon pipe_inner1_polygon = GetInner1Pipe();
    pipe_inner1_.addPolygon(pipe_inner1_polygon);
    QPolygon pipe_inner2_polygon = GetInner2Pipe();
    pipe_inner2_.addPolygon(pipe_inner2_polygon);
}

void BeamLine::BuildBeamErasers() {
    for (Constants::BEAMLINE_PATHS path : beam_paths_.keys()) {
        BeamPath* beam_path = beam_paths_[path];
        QPolygon polygon = beam_path->Path(beam_path->TotalLength());
        beam_erasers_[path].addPolygon(polygon);
    }
}

}

