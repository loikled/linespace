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
                  FREE = 1,
                  DRAW = 2,
                  EDIT = 3,
                  MENU = 4} CursorState_e;

private:
    static const GLfloat colors_[5][4];
    Leap::Vector pos_;
    CursorState_e state_;
    float size_;
    float sensitivity_;

public:
    explicit Cursor(QObject *parent = 0);
    const GLfloat* getColorFromState() const;
    float getSize() const;
    const Leap::Vector getPos() const;

signals:

public slots:
    void slotMove(const Leap::Vector& pos);
    void slotChangeState(Cursor::CursorState_e state);

};

#endif // CURSOR_H
