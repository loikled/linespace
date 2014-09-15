#include "shape.h"

#define LINE  0
#define CIRCLE 1
#define POLYGONE 2
#define HELICOIDE 3


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
        center_ = Leap::Vector(0,0,0);
        direction_ = Leap::Vector(1,0,0);
        normal_ = Leap::Vector(0,0,1);
        size_ = 0.8;
        nbSegment_ = 150;
        createCircle();
        break;
    case POLYGONE :
        center_ = Leap::Vector(0,0,0);
        direction_ = Leap::Vector(1,0,0);
        normal_ = Leap::Vector(0,0,1);
        size_ = 0.8;
        nbSegment_ = 4;
        createCircle();
    case HELICOIDE :
        center_ = Leap::Vector(0,0,0);
        direction_ = Leap::Vector(1,0,0);
        normal_ = Leap::Vector(0,0,1);
        size_ = 0.4;
        nbSegment_ = 150;
        length_ = 0.5;
        step_ = 10;
        HelicoideAngle_ = -40;
        createHelicoide();
    default :
        break;
    }
}

void Shape::createCircle()
{
    lineList_.clear();
    Leap::Vector* vectNull = new Leap::Vector(0,0,0);

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
    createCircle();
}

void Shape::changeCircleCenter(Leap::Vector center)
{
    center_ = center;
    createCircle();
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

QList<Shape::line_t> Shape::getList()
{
    return lineList_;
}
