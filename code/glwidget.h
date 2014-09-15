#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QImage>
#include <QList>
#include <QDir>
#include <QMutex>

#include "leapmotion/LeapListener.h"
#include "camera.h"
#include "grid.h"
#include "glview.h"
#include "tracking_defines.h"
#include "cursor.h"
#include "shape.h"

#define NB_TEXTURE 2
#define BOX_SIZE 8.0f //the grid is always inside the box

using namespace Leap;

class GlWidget : public Glview
{
    Q_OBJECT
public:
    //typedef for textures management
    typedef enum {CRATE,
                  METAL,
                  NONE = -1} texId_t;

    //simple way of describing a cube/item
    struct item_t {
        float x_;
        float y_;
        float z_;
        float size_;
        float sizeOffset_;
        float yOffset_;
        texId_t texture_;
        bool selected_;
        bool drawn_;
        //constructor
        item_t(float pSize = 1.0f, texId_t pText = CRATE);
    };    
    typedef QSet<int> GrabList_t;

private:
    LeapListener leapListener_;
    Leap::Controller controller_;

    GLuint texture_[NB_TEXTURE];

    //record and display timer and maxValueTimer
    QTime recordTimer_;
    int maxRecordTimer_;
    int curentRecordTime_;
    int lastElapsedTime_;
    bool recording_;
    bool playing_;
    Shape shape_;
    bool isPinchRight_ = false;
    bool isPinchLeft_ = false;
    bool newShape_ = false;

    Cam cam_;
    Cursor cursor_;

    Leap::Vector palmPos_;
    Leap::Vector lastFingerPos;
    QList<Shape> shapeList_;
    HandEvent::Selection_t selectionMode_;
    float boxSize_;
    int gridSize_;
    float spacing_;
    float zoomOffset_;
    float maxZoom_;
    bool grabbing_;


    mutable QMutex mutexList_;
    GrabList_t grabList_;
    QList<Leap::Vector> lastPos_;
    Grid grid_;

public:
    GlWidget(QWidget *parent = 0);
    ~GlWidget();

    //writing allowed or not TODO change the way to write or not write
     bool writing_;

    //recording function to start or stop timer, or set new value to the timer
    void startRecord();
    void stopRecord();
    void play();
    void setNewTime(int time);

    //opengl functions
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void loadTexture(QString textureName, texId_t pId);
    void drawCube(texId_t PtextureId,
                  float pCenterX,
                  float pCenterY,
                  float pCenterZ,
                  float pSize);
    void drawCube(const item_t& pCube);
    void clearCurves();

private:
    void handleGrab();
    void customEvent(QEvent* pEvent);
    void drawGrid();
    void drawCurve(const Shape shape);
    void drawCursor();
    void drawCylinder(Leap::Vector point1, Leap::Vector point2);
    void drawFocus();

signals:
    void setTimeAndTotalTime(int currentTime, int totalTime);

public slots:
    void slotNewHead(head_t pPos);
    void slotMoveHead(int pAxis, float pDelta);
    void slotChangeGains(Leap::Vector gains);
private slots:
};

#endif // GLWIDGET_H
