#ifndef CAMERA_H
#define CAMERA_H
#include "leapmotion/Leap.h"
#include <QObject>

class Cam : public QObject{
 Q_OBJECT
    private:
        Leap::Vector pos_; //actual coordinates of the cam
        Leap::Vector angleFactor_; //magnitude of movement from head position
        Leap::Vector focus_; //point that is put in the center of the screen
        float zoom_; // controls size of the field of view

    public:
        Cam();
        virtual ~Cam();
        void setFocus(Leap::Vector& focus);
        Leap::Vector getPos() const;
        Leap::Vector getFocus() const;

    public slots:
        void slotUpdateFromHeadPos(const Leap::Vector& HeadPos);
        void slotTranslate(const Leap::Vector& offset);
        void slotChangeFactor(const Leap::Vector& gains);
};

#endif // CAMERA_H
