#ifndef LEAPLISTENER_H
#define LEAPLISTENER_H

#include <QObject>
#include <QTimer>

#include "leapmotion/Leap.h"
#include "leapmotion/HandEvent.h"

using namespace Leap;

class LeapListener: public QObject, public Leap::Listener
{
    Q_OBJECT

public:
    //state machine for clic like gestures
    typedef enum {OPEN, CLOSE} handOpening_t;

private:
    // remember the id of item in leap frames
    int32_t rightHand_;
    int32_t leftHand_;

    // id of item near the hand
    int trackedItem_;
    bool trackPrevious_;
    handOpening_t handState_;
    float zoomFactor_;
    bool grabbing_;

    //right hand palm position
    Leap::Vector rPos_;

    // finger which draw on the right hand
    Leap::Vector fingerPos_;
    //Active or not writting
    bool writing_;

    HandEvent::Selection_t  selectionMode_;
    QObject* receiver_;
    QTimer* swipeTimer_;

public:
    //ctor
    LeapListener();
    void setReceiver(QObject* pObject);

    //leap listener functions
    void onInit(const Controller&);
    void onConnect(const Controller&);
    void onDisconnect(const Controller&);
    void onExit(const Controller&);
    void onFrame(const Controller&);

    Leap::Vector getFingerPos();

    /*update the tracked item
     *called from the collision
     *detector
     */
    void setItem(int pNewItem);
    void detectGesture(const Frame& pFrame);

private:
    //encapsulated functions for sending events
    void openEvent();
    void closeEvent();
    void zoomEvent();
    void clickEvent();
    void doubleClickEvent();
    void swipeEvent();
    void grabEvent();
    void moveEvent();
    void circleEvent();
};

#endif // LEAPLISTENER_H
