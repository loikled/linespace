#ifndef SHAPE_H
#define SHAPE_H


#include <QList>
#include "leapmotion/Leap.h"

using namespace Leap;

class Shape
{

public:
    typedef enum {
        CURVE = 0,
        LINE = 1,
        CIRCLE = 2,
        HELICOIDE = 3,
        POLYGON = 4
    } Shape_t;


public:
    struct line_t {
        Leap::Vector firstPoint_;
        Leap::Vector secondPoint_;
        //constructor
        line_t(Leap::Vector firstPoint, Leap::Vector secondPoint){firstPoint_ = firstPoint;secondPoint_ = secondPoint;}
    };

    Shape();
    void newType(int type);
    void changeLeft(Leap::Vector left);
    void changeRight(Leap::Vector right);
    void changeCircleSize(float size);
    void changeCircleCenter(Leap::Vector center);
    void changeCircleDirection(Leap::Vector direction);
    void changeCircleNormal(Leap::Vector normal);
    void createCircle();
    void createHelicoide();
    void addLine(line_t line);
    const QList<line_t> getList() const;

private:
    QList<line_t>  lineList_;
    Leap::Vector center_;
    Leap::Vector direction_;
    Leap::Vector normal_;
    int nbSegment_ = 1;
    float length_ = 0.5;
    int step_ = 1;
    // angle in degre
    float HelicoideAngle_ = 0;
    float size_;

};

#endif // SHAPE_H
