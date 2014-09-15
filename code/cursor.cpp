#include "cursor.h"
#include "qdebug.h"

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
    mode_(Cursor::CURVE),
    size_(0.02f),
    sensitivity_(2.0f),
    state_(0)
{
}

const GLfloat* Cursor::getColorFromState() const{
    return colors_[mode_];
}

const Leap::Vector Cursor::getPos() const{
    return pos_;
}

Cursor::CursorMode_e Cursor::getMode() const{
    return mode_;
}

int Cursor::getState() const{
    return state_;
}

const Leap::Vector Cursor::getPos(const Leap::Vector& pos) const{
    Leap::Vector offset(-0.5f, -0.5f, -1.0f);
    return (pos + offset)*sensitivity_;
}

Shape::line_t Cursor::getLastMove()
{
    return Shape::line_t(lastFingerPos_, pos_);
}

float Cursor::getSize() const{
    return size_;
}

//update position from normalized leap unit box to our coordinate space
void Cursor::slotMove(const Leap::Vector& pos){
    //Leap normalized is in range [0..1]
    //we want to center on 0,0 and Z in range [-1, 0]
    Leap::Vector offset(-0.5f, -0.5f, -1.0f);
    lastFingerPos_ = pos_;
    pos_ = (pos + offset)*sensitivity_;

}

void Cursor::changeState(int state)
{
    state_ = state;
}

void Cursor::slotNextMode(){
    switch(mode_){
        case CURVE:
            mode_ = CIRCLE;
            break;
        case CIRCLE:
            mode_ = SEGMENT;
            break;
       case SEGMENT:
            mode_ = MOVE;
            break;
       case MOVE:
            mode_ = FOCUS;
            break;
       case FOCUS:
            mode_ = CURVE;
            break;
       default:
            mode_ = CURVE;
            break;
   }
    state_ = 0;

}
