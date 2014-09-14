#ifndef SHAPE_H
#define SHAPE_H


#include <QList>
#include "leapmotion/Leap.h"

using namespace Leap;

class Shape
{



public:
    struct line_t {
        Leap::Vector firstPoint_;
        Leap::Vector secondPoint_;
        int timePainted_;
        //constructor
        line_t(Leap::Vector firstPoint, Leap::Vector secondPoint, int timePainted){firstPoint_ = firstPoint;secondPoint_ = secondPoint;timePainted_ = timePainted;}
        line_t(Leap::Vector firstPoint, Leap::Vector secondPoint){firstPoint_ = firstPoint;secondPoint_ = secondPoint;timePainted_ = 0;}
    };

    Shape();
    void newType(int type);
    void changeLeft(Leap::Vector left);
    void changeRight(Leap::Vector right);
    QList<line_t> getList();

private:
    QList<line_t>  lineList_;
    Leap::Vector center;
    Leap::Vector direction;
    Leap::Vector normal;
    float size_;

};

#endif // SHAPE_H
