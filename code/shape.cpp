#include "shape.h"

Shape::Shape()
{
}

void Shape::newType(int type)
{
    lineList_.clear();
    switch(type)
    {
        case Shape::CURVE :

        break;
        case Shape::LINE :
            //lineList_.append(line_t(Leap::Vector(-0.25,0,0),Leap::Vector(0.25,0,0)));
        break;

        case Shape::CIRCLE :
        center_ = Leap::Vector(0,0,0);
        direction_ = Leap::Vector(0,0,1);
        normal_ = Leap::Vector(1,0,0);
        size_ = 0.2;
        nbSegment_ = 150;
        createCircle();
        break;

    default :
        break;
    }
}

void Shape::createCircle()
{
    lineList_.clear();
    float angle = 2*PI/nbSegment_;
   for(int i = 0; i<nbSegment_;i++)
   {
       Leap::Vector firstPoint = center_ + cos(angle*i)*size_*direction_.normalized() + sin(angle*i)*size_*normal_.normalized();
       Leap::Vector secondPoint = center_ + cos(angle*(i+1))*size_*direction_.normalized() + sin(angle*(i+1))*size_*normal_.normalized();
       lineList_.append(line_t(firstPoint,secondPoint));
   }
}

void Shape::createHelicoide()
{
    lineList_.clear();

   float angle = 2*PI/nbSegment_;
   Leap::Vector helicoideDirection = -direction_.cross(normal_);
   float stepInterval = length_/nbSegment_/step_;
   float sizeInterval = length_*sin(HelicoideAngle_/180*PI)/nbSegment_/step_;

   for(int j=0;j<step_;j++)
   {
       for(int i = 0; i<nbSegment_;i++)
       {
           float firstModifiedSize = (1+sizeInterval*(i+j*nbSegment_));
           float secondModifiedSize = (1+sizeInterval*(i+1+j*nbSegment_));
           Leap::Vector firstPoint = center_ + cos(angle*i)*size_*firstModifiedSize*direction_.normalized() + sin(angle*i)*size_*firstModifiedSize*normal_.normalized() + helicoideDirection.normalized()*stepInterval*(i+j*nbSegment_);
           Leap::Vector secondPoint = center_ + cos(angle*(i+1))*size_*secondModifiedSize*direction_.normalized() + sin(angle*(i+1))*size_*secondModifiedSize*normal_.normalized() + helicoideDirection.normalized()*stepInterval*(i+1+j*nbSegment_);
           lineList_.append(line_t(firstPoint,secondPoint));
       }
   }
}

void Shape::changeLeft(Leap::Vector left)
{
    Leap::Vector right = Leap::Vector::zero();
    if(lineList_.length()>0){
        right = lineList_[0].secondPoint_;
    }
        lineList_.clear();
        lineList_.append(line_t(left,right));
}

void Shape::changeRight(Leap::Vector right)
{
    Leap::Vector left = Leap::Vector::zero();
    if(lineList_.length()>0){
      left = lineList_[0].firstPoint_;
    }
        lineList_.clear();
        lineList_.append(line_t(left,right));
}

void Shape::changeCircleSize(float size)
{
    size_ = size;
    createCircle();
}

void Shape::changeCircleCenter(Leap::Vector center)
{
    center_ = center;
    createCircle();
}

Leap::Vector Shape::getCircleCenter()
{
    return center_;
}

void Shape::changeCircleDirection(Leap::Vector direction)
{
    direction_ = direction;
    createCircle();
}

void Shape::changeCircleNormal(Leap::Vector normal)
{
    normal_ = normal;
    createCircle();
}

void Shape::addLine(line_t line)
{
    lineList_.append(line);
}

const QList<Shape::line_t> Shape::getList() const
{
    return lineList_;
}

void Shape::translate(Leap::Vector offset){
    for(line_t& l : lineList_){
        l.firstPoint_ += offset;
        l.secondPoint_ += offset;
    }
    center_ += offset;
}
