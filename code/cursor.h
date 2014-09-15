#ifndef CURSOR_H
#define CURSOR_H
#include <GL/gl.h>

#include <QObject>
#include "leapmotion/Leap.h"
#include "shape.h"

class Cursor : public QObject
{
    Q_OBJECT

public:
    typedef enum {
                  CURVE = 1,
                  CIRCLE = 2,
                  SEGMENT = 3,
                  EDIT = 4,
                  LAST} CursorMode_e;

    typedef enum {
        IDLE = 0,
        STATE1 = 1,
        STATE2 = 2
    } CursorState_e;

private:
    static const GLfloat colors_[5][4];
    Leap::Vector pos_;

    // correspond to both pinch left and right hand
    CursorMode_e mode_;
    int state_;

    float size_;
    float sensitivity_;

    //to draw a line
    Leap::Vector lastFingerPos_;

public:
    explicit Cursor(QObject *parent = 0);
    const GLfloat* getColorFromState() const;
    float getSize() const;
    const Leap::Vector getPos() const;
    const Leap::Vector getPos(const Leap::Vector& pos) const;
    Cursor::CursorMode_e getMode() const;
    int getState() const;
    void changeState(int state);
    Shape::line_t getLastMove();
    // Leap events
    bool isPinchLeft_;
    bool isPinchRight_;
    Leap::Vector leftPinchPosition_;
    Leap::Vector rightPinchPosition_;


signals:

public slots:
    void slotMove(const Leap::Vector& pos);
    void slotNextMode();

};

#endif // CURSOR_H
