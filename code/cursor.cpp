#include "cursor.h"

const GLfloat Cursor::colors_[5][4] = {
    {1.0, 1.0, 1.0, 0.8},
    {0.0, 0.0, 0.0, 1.0},
    {1.0, 0.0, 0.0, 0.7},
    {0.0, 1.0, 0.0, 0.7},
    {0.0, 0.0, 1.0, 0.7},
};

Cursor::Cursor(QObject *parent) :
    QObject(parent),
    pos_(Leap::Vector()),
    state_(Cursor::IDLE),
    size_(0.02f)
{
}

const GLfloat* Cursor::getColorFromState() const{
    return colors_[state_];
}

const Leap::Vector Cursor::getPos() const{
    return pos_;
}

float Cursor::getSize() const{
    return size_;
}

void Cursor::slotMove(const Leap::Vector& pos){
    pos_ = pos;
}

void Cursor::slotChangeState(Cursor::CursorState_e state){
    state_ = state;
}
