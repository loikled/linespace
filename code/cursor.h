#ifndef CURSOR_H
#define CURSOR_H
#include <GL/gl.h>

#include <QObject>
#include "leapmotion/Leap.h"

class Cursor : public QObject
{
    Q_OBJECT

public:
    typedef enum {IDLE = 0,
                  CURVE = 1,
                  CIRCLE = 2,
                  SEGMENT = 3,
                  EDIT = 4,
                  LAST} CursorMode_e;
    typedef enum {
        MOVE = 0,
        CHANGE = 1

    } CursorStep_e;
private:
    static const GLfloat colors_[5][4];
    Leap::Vector pos_;

    // correspond to both pinch left and right hand
    CursorMode_e mode_;
    float size_;
    float sensitivity_;


public:
    explicit Cursor(QObject *parent = 0);
    const GLfloat* getColorFromState() const;
    float getSize() const;
    const Leap::Vector getPos() const;
    const Leap::Vector getPos(const Leap::Vector& pos) const;
    Cursor::CursorMode_e getMode() const;

signals:

public slots:
    void slotMove(const Leap::Vector& pos);
    void slotChangeState(Cursor::CursorMode_e mode);
    void slotNextMode();

};

#endif // CURSOR_H
