#include "leapmotion/LeapListener.h"
#include "leapmotion/HandEvent.h"

#include <QApplication>
#include <QDebug>


#define SELECT_TRESHOLD 60.0f //hand opening in mm
#define RELEASE_TRESHOLD 80.0f //hand opening in mm
#define HOLD_TIME 10 //nb of frame with hand closed

#define ANGLE_ZOOM_TRESHOLD 20.0f // pitch of left hand in degrees
#define ZOOM_FACTOR 0.01f // each frame in zoom moves by this.
#define RESWIPE_INTERVAL 200 //minimum time between 2 swipes in ms

LeapListener::LeapListener()
    : rightHand_(-1),
      leftHand_(-1),
      trackedItem_(-1),
      trackPrevious_(false),
      handState_(OPEN),
      zoomFactor_(0)
{
    swipeTimer_ = new QTimer();
    swipeTimer_->setInterval(RESWIPE_INTERVAL);
    swipeTimer_->setSingleShot(true);
}

void LeapListener::onInit(const Controller& controller)
{
     qDebug() << "Initialized";
     Q_UNUSED(controller);
     /*Config config = controller.config();
     config.setFloat("Gesture.Circle.MinRadius", 30);
     config.setFloat("Gesture.Circle.MinArc", 2*PI);
     config.save(); */
}

void LeapListener::onConnect(const Controller& controller)
{
    qDebug() << "Connected";
    controller.enableGesture(Gesture::TYPE_KEY_TAP);
    controller.enableGesture(Gesture::TYPE_SWIPE);
    //controller.enableGesture(Gesture::TYPE_CIRCLE);
}

void LeapListener::onDisconnect(const Controller& controller)
{
    Q_UNUSED(controller);
    qDebug() << "Disconnected";
}

void LeapListener::onExit(const Controller& controller)
{
    Q_UNUSED(controller);
    qDebug() << "Exited";
}


Leap::Vector middleVector(Vector vect1, Vector vect2)
{
        return Vector((vect1.x + vect2.x)/2, (vect1.y + vect2.y)/2, (vect1.z + vect2.z)/2);
}

void LeapListener::onFrame(const Controller& controller)
{
    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();
    Vector fingerpos;




    Hand rightHand = frame.hands().rightmost();
    Hand leftHand = frame.hands().leftmost();

    if (rightHand.isRight() && rightHand.isValid())
    {
        rightHand_ = rightHand.id();

        Finger rightIndex = rightHand.fingers().fingerType(Finger::TYPE_INDEX)[0];
        Finger rightThumb = rightHand.fingers().fingerType(Finger::TYPE_THUMB)[0];

        if(frame.hands().count() == 1 && rightIndex.isExtended()){
            fingerpos = rightIndex.tipPosition();
            if(rightThumb.tipPosition().distanceTo(rightHand.arm().wristPosition()) < 97.0f)
            {
                writing_ = true;
            }
            else
            {
                writing_ = false;
            }
            InteractionBox box = frame.interactionBox();
            if ( box.isValid() )
                fingerPos_ = box.normalizePoint(fingerpos, false);

            //always send a move event
            moveEvent();
            detectGesture(frame);
            }


        // new event to pinch with thumb and index right
        if(rightThumb.tipPosition().distanceTo(rightIndex.tipPosition()) < 50.0f)
        {
            InteractionBox box = frame.interactionBox();
            Leap::Vector vect = (rightThumb.tipPosition() + rightIndex.tipPosition())/2;
            if ( box.isValid() )
               pinchEvent(false,box.normalizePoint(vect, false));

        }
        else
        {
         releaseEvent(false);
        }

        // Circle event
        Leap::Vector direction = Leap::Vector(cos(rightHand.direction().roll()),sin(rightHand.direction().roll()),0);
        Leap::Vector normal = Leap::Vector(0, sin(rightHand.direction().pitch()),-cos(rightHand.direction().pitch()));
        InteractionBox box = frame.interactionBox();
        if ( box.isValid() )
            circleEvent(box.normalizePoint(rightHand.sphereCenter(),false), direction, normal, rightHand.sphereRadius());




    }
    else
    {
        writing_ = false;
    }

    if (leftHand.isLeft() && leftHand.isValid())
    {
        Finger leftIndex = leftHand.fingers().fingerType(Finger::TYPE_INDEX)[0];
        Finger leftThumb = leftHand.fingers().fingerType(Finger::TYPE_THUMB)[0];

        float angle = leftHand.palmNormal().roll()*180.0f/PI;

        if(abs(angle)>25.0f && frame.hands().count() == 1)
        {
            sliderEvent(angle);
        }


        // new event to pinch with thumb and index right
        if(leftThumb.tipPosition().distanceTo(leftIndex.tipPosition()) < 50.0f )
        {

            Leap::Vector vect = (leftThumb.tipPosition() + leftIndex.tipPosition())/2;
            InteractionBox box = frame.interactionBox();
            if ( box.isValid() )
                pinchEvent(true,box.normalizePoint(vect, false));
        }
        else
        {
            releaseEvent(true);
        }

    }




}

void LeapListener::detectGesture(const Frame& pFrame)
{
    GestureList list = pFrame.gestures();
    if (list.count() > 0 && list[0].isValid() )
    {
        Gesture gest = list[0];
        switch ( gest.type() )
        {
        case Gesture::TYPE_INVALID:
            break;
        case Gesture::TYPE_KEY_TAP:
            if ( gest.hands().rightmost().isRight())
                clickEvent();
            break;
        case Gesture::TYPE_SWIPE:
            if ( gest.hands()[0].id() == rightHand_
                && gest.state() == Gesture::STATE_STOP
                && !swipeTimer_->isActive() )
            {
                SwipeGesture swipe = SwipeGesture(gest);
                // direction almost vertical
                float angle = swipe.direction().angleTo(Vector(-1,0,0))*180.0f/PI;
                if (angle <= 30.0f)
                {
                    swipeEvent();
                    swipeTimer_->start();
                }
            }
            break;
        case Gesture::TYPE_CIRCLE:
            if ( gest.hands()[0].id() == rightHand_ &&
                 gest.state() == Gesture::STATE_STOP )
                //circleEvent();
            break;
        default:
            break;
        }
    }
}

void LeapListener::setReceiver(QObject* pObject)
{
    receiver_ = pObject;
}

void LeapListener::setItem(int pNewItem)
{
    if ( !(trackedItem_ == pNewItem) )
    {
        // if you leave an item with closed hand = you grab it
        if (handState_ == CLOSE && trackedItem_ != -1)
        {
            grabbing_ = true;
            grabEvent();
        }
        trackPrevious_ = false;
        trackedItem_ = pNewItem;
    }
}

void LeapListener::openEvent()
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Opened, rPos_, trackedItem_);
        QApplication::postEvent(receiver_, event);
    }
}

void LeapListener::closeEvent()
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Closed, rPos_, trackedItem_);
        QApplication::postEvent(receiver_,event);
    }
}

void LeapListener::zoomEvent()
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Zoom, rPos_, trackedItem_, selectionMode_, zoomFactor_);
        QApplication::postEvent(receiver_,event);
    }
}

void LeapListener::clickEvent()
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Clicked, rPos_, trackedItem_, selectionMode_);
        QApplication::postEvent(receiver_,event);
    }
}

void LeapListener::doubleClickEvent()
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::DoubleClicked, rPos_, trackedItem_, selectionMode_);
        QApplication::sendEvent(receiver_,event);
    }
}

void LeapListener::swipeEvent()
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Swiped, rPos_, trackedItem_);
        QApplication::postEvent(receiver_,event);
    }
}

void LeapListener::grabEvent()
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Grabbed, rPos_, trackedItem_);
        QApplication::postEvent(receiver_, event);
    }
}

void LeapListener::moveEvent()
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Moved, fingerPos_);
        event->writing(writing_);
        QApplication::postEvent(receiver_, event);
    }
}

void LeapListener::circleEvent(Leap::Vector center, Leap::Vector direction, Leap::Vector normal, float size)
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Circle, center);
        event->circleDirection_ = direction;
        event->circleNormal_ = normal;
        event->circleSize_ = size;
        QApplication::postEvent(receiver_, event);
    }
}

void LeapListener::sliderEvent(float angle)
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Slider);
        event->sliderAngle(angle);
        QApplication::postEvent(receiver_, event);
    }
}

void LeapListener::pinchEvent(bool isLeft, Vector pinchPos)
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Pinch,pinchPos);
        event->pinchIsLeft(isLeft);
        QApplication::postEvent(receiver_, event);
    }
}

void LeapListener::releaseEvent(bool isLeft)
{
    if ( receiver_ )
    {
        HandEvent* event = 0;
        event = new HandEvent(HandEvent::Release);
        event->pinchIsLeft(isLeft);
        QApplication::postEvent(receiver_, event);
    }
}
