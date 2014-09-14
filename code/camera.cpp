#include "camera.h"
#include "math.h"
#include <QDebug>

const float Cam::moveOffset_ = 0.05f;

Cam::Cam()
    :pos_(Leap::Vector(0.0, 0.0, 1.5)),
     angleFactor_(Leap::Vector(1.0, 1.0, 1.0)),
     focus_(Leap::Vector(0, 0, 0)),
     zoom_(1.0f),
    focusIndex_(0)
{
}

Cam::~Cam(){}

void Cam::focusUp(int max){
    if (focusIndex_ < max)
        focusIndex_++;
    else
        focusIndex_ = max;
}

void Cam::focusDown(){
   if (focusIndex_ > 0)
       focusIndex_--;
   else
       focusIndex_ = 0;
}

void Cam::resetFocus(){
    focusIndex_ = 0;
    focus_ = Leap::Vector(0,0,0);
}

uint Cam::getFocusIndex(){
    return focusIndex_;
}

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

void Cam::slotChangeFactor(const Leap::Vector& gains){
    angleFactor_ = gains;
}

//convert head coordinates from desk space in front of screen to
//camera coordinates in the screen, taking into account angle multipliers
//headposition is in theta,phy,r format
void Cam::slotUpdateFromHeadPos(const Leap::Vector& HeadPos)
{
    qDebug()<<"Head z: "<< HeadPos.z;
    float oldZ = pos_.z;
    static float targetZ = 1.0f;
    targetZ = HeadPos.z*angleFactor_.z + focus_.z + zoom_;

    float delta = targetZ - oldZ;
    delta = delta < 0 ? -delta : delta;
    if (delta > moveOffset_)
        pos_.z = oldZ + (targetZ - oldZ)/5.0f;
    float depth = abs(pos_.z - focus_.z);
    pos_.x = -tan(HeadPos.x * angleFactor_.x)*depth + focus_.x;
    pos_.y = -tan(HeadPos.y * angleFactor_.y)*depth + focus_.y;
}
