#include "leapmotion/HandEvent.h"

//types must be greater than User
HandEvent::HandEvent(QEvent::Type pType,
          Vector pPos,
          int pSelect,
          Selection_t pSelectMode,
          float pZoom)
    : QEvent(pType),
      pos_(pPos),
      itemSelected_(pSelect),
      selection_(pSelectMode),
      zoomOffset_(pZoom)
{
}

Vector HandEvent::pos()
{
    return pos_;
}

bool HandEvent::writting()
{
    return writting_;
}

void HandEvent::writting(bool isWritting)
{
    writting_ = isWritting;
}

int HandEvent::item()
{
    return itemSelected_;
}

void HandEvent::sliderAngle(float angle)
{
    sliderAngle_ = angle;
}

float HandEvent::sliderAngle()
{
    return sliderAngle_;
}

void HandEvent::pinchIsLeft(bool isLeft)
{
    pinchIsLeft_ = isLeft;
}

bool HandEvent::pinchIsLeft()
{
    return pinchIsLeft_;
}



HandEvent::Selection_t HandEvent::selectMode()
{
    return selection_;
}

float HandEvent::zoom()
{
    return zoomOffset_;
}
