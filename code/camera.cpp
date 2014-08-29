#include "camera.h"
#include "math.h"

Cam::Cam()
    :pos_(Leap::Vector(0.0, 0.0, 1.0)),
     angleFactor_(Leap::Vector(1.0, 1.0, 1.0)),
     focus_(Leap::Vector(0, 0, 0)),
     zoom_(1.0f)
{
}

Cam::~Cam(){}

void Cam::setFocus(Leap::Vector& focus){
    focus_ = focus;
}

Leap::Vector Cam::getPos() const {
    return pos_;
}

Leap::Vector Cam::getFocus() const {
    return focus_;
}

void Cam::slotTranslate(const Leap::Vector& offset){
    pos_+= offset;
}

void Cam::slotChangeFactor(float theta, float phy, float r){
    angleFactor_ = Leap::Vector(theta,phy,r);
}

//convert head coordinates from desk space in front of screen to
//camera coordinates in the screen, taking into account angle multipliers
//headposition is in theta,phy,r format
void Cam::slotUpdateFromHeadPos(const Leap::Vector& HeadPos)
{
    pos_.x = sin(HeadPos.x)*angleFactor_.x + focus_.x;
    pos_.y = cos(HeadPos.y)*angleFactor_.y + focus_.y;
    pos_.z = HeadPos.z - focus_.z*angleFactor_.z;
}
