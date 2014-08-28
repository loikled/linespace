#ifndef GRID_H
#define GRID_H

#include <QObject>
#include <QList>
#include <QPair>
#include "leapmotion/Leap.h"

class Grid : public QObject
{
    Q_OBJECT

public:
    typedef QPair<Leap::Vector, Leap::Vector> GridLine_t;
    typedef QList<GridLine_t> GridList_t;

private:
    static const float defaultUnit;
    float unit_; //space between 2 lines (assumes that screen is from 0 to 1)
    Leap::Vector left_; //borders of the screen
    Leap::Vector right_;
    bool fill_;
    GridList_t grid_;

public:
    explicit Grid(QObject *parent = 0);
    void computeLines();
    void setUnit(float unit);
    void setBorders(Leap::Vector& left, Leap::Vector& right);
    void enableFill(bool fill = true);
    void DrawPlaneXY(float z);
    void DrawPlaneXZ(float y);
    void DrawPlaneYZ(float x);
    const GridList_t* getLineList() const;

signals:

public slots:

};

#endif // GRID_H
