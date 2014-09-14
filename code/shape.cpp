#include "shape.h"

#define LINE  0
#define CIRCLE 1



Shape::Shape()
{
}

void Shape::newType(int type)
{
    lineList_.clear();
    switch(type)
    {
        case LINE :
            lineList_.append(line_t(Leap::Vector(-0.25,0,0),Leap::Vector(0.25,0,0)));
        break;

        case CIRCLE :
        center = Leap::Vector(0,0,0);
        direction = Leap::Vector(0.4,0,0);
        normal = Leap::Vector(0,0,0.4);
        break;
    default :
        break;
    }
}

void setCircle(int nbSegment)
{
       for(int i = 0; i<nbSegment;i++){

       }
}

void Shape::changeLeft(Leap::Vector left)
{
    if(lineList_.length()>0){
        Leap::Vector right = lineList_[0].secondPoint_;
        lineList_.clear();
        lineList_.append(line_t(left,right));
    }
}

void Shape::changeRight(Leap::Vector right)
{
    if(lineList_.length()>0){
        Leap::Vector left = lineList_[0].firstPoint_;
        lineList_.clear();
        lineList_.append(line_t(left,right));
    }
}

QList<Shape::line_t> Shape::getList()
{
    return lineList_;
}