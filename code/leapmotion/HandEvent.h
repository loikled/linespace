#ifndef HANDEVENT_H
#define HANDEVENT_H

#include <QEvent>
#include "leapmotion/Leap.h"

using namespace Leap;

//Custom events must be > QEvent::User (1000)
#define EVENT_OPEN 1002
#define EVENT_CLOSE 1003
#define EVENT_CLICK 1004
#define EVENT_DOUBLE_CLICK 1005
#define EVENT_ZOOM 1006
#define EVENT_SWIPE 1007
#define EVENT_GRAB 1008
#define EVENT_MOVE 1009
#define EVENT_CIRCLE 1010
#define EVENT_SLIDER 1011
#define EVENT_PINCH 1012
#define EVENT_RELEASE 1013

//defining a custom Event
class HandEvent: public QEvent
{
public:

    typedef enum {SINGLE, MULTIPLE} Selection_t;

    //create naming for our custom events
    static const QEvent::Type Opened = static_cast<QEvent::Type>(EVENT_OPEN);
    static const QEvent::Type Closed = static_cast<QEvent::Type>(EVENT_CLOSE);
    static const QEvent::Type Clicked = static_cast<QEvent::Type>(EVENT_CLICK);
    static const QEvent::Type DoubleClicked = static_cast<QEvent::Type>(EVENT_DOUBLE_CLICK);
    static const QEvent::Type Zoom = static_cast<QEvent::Type>(EVENT_ZOOM);
    static const QEvent::Type Swiped = static_cast<QEvent::Type>(EVENT_SWIPE);
    static const QEvent::Type Grabbed = static_cast<QEvent::Type>(EVENT_GRAB);
    static const QEvent::Type Moved = static_cast<QEvent::Type>(EVENT_MOVE);
    static const QEvent::Type Circle = static_cast<QEvent::Type>(EVENT_CIRCLE);
    static const QEvent::Type Slider = static_cast<QEvent::Type>(EVENT_SLIDER);
    static const QEvent::Type Pinch = static_cast<QEvent::Type>(EVENT_PINCH);
    static const QEvent::Type Release = static_cast<QEvent::Type>(EVENT_RELEASE);

private:
    Leap::Vector pos_; // index tip position
    bool writing_ = false;
    int itemSelected_;
    Selection_t selection_;
    float zoomOffset_;
    float sliderAngle_;
    bool pinchIsLeft_ = false;


public:
    //types must be greater than User
    HandEvent(QEvent::Type pType = QEvent::User,
              Leap::Vector pPos = Leap::Vector(),
              int pSelect = -1,
              Selection_t pSelectMode = SINGLE,
              float pZoom = 0.0f);
    Leap::Vector pos();
    bool writing();
    void writing(bool isWritting);
    float sliderAngle();
    void sliderAngle(float angle);
    bool pinchIsLeft();
    void pinchIsLeft(bool isLeft);

    int item();
    HandEvent::Selection_t selectMode();
    float zoom();

    //Public variables to draw circle. Too many functions to create then variables are set to public
    Leap::Vector circleDirection_;
    Leap::Vector circleNormal_;
    float circleSize_;

};




#endif // HANDEVENT_H
