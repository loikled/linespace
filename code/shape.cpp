#include "shape.h"

Shape::Shape()
{
}

void Shape::newType(int type)
{
    lineList_.clear();
    switch(type)
    {
        case Shape::LINE :
            lineList_.append(line_t(Leap::Vector(-0.25,0,0),Leap::Vector(0.25,0,0)));
        break;

        case Shape::CIRCLE :
        center_ = Leap::Vector(0,0.1,0);
        direction_ = Leap::Vector(0.4,-0.3,0.1);
        normal_ = Leap::Vector(0,-0.30,0.4);
        size_ = 0.8;
        setCircle(100);
        break;
    default :
        break;
    }
}

void Shape::setCircle(int nbSegment)
{
    lineList_.clear();
    Leap::Vector* vectNull = new Leap::Vector(0,0,0);

    float angle = 2*PI/nbSegment;
   for(int i = 0; i<nbSegment;i++){
       Leap::Vector firstPoint = center_ + cos(angle*i)*size_/vectNull->distanceTo(direction_)*direction_ + sin(angle*i)*size_/vectNull->distanceTo(normal_)*normal_;
       Leap::Vector secondPoint = center_ + cos(angle*(i+1))*size_/vectNull->distanceTo(direction_)*direction_ + sin(angle*(i+1))*size_/vectNull->distanceTo(normal_)*normal_;
       lineList_.append(line_t(firstPoint,secondPoint));
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

void Shape::changeCircleSize(float size)
{
    size_ = size;
    setCircle(100);
}

void Shape::changeCircleCenter(Leap::Vector center)
{
    center_ = center;
    setCircle(100);
}

void Shape::changeCircleDirection(Leap::Vector direction)
{
    direction_ = direction;
    setCircle(100);
}

void Shape::changeCircleNormal(Leap::Vector normal)
{
    normal_ = normal;
    setCircle(100);
}

const QList<Shape::line_t> Shape::getList() const
{
    return lineList_;
}
