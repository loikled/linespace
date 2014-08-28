#include "glwidget.h"

#include <math.h>
#include <GL/glu.h>
#include <iostream>
#include <QMutexLocker>

#include "leapmotion/HandEvent.h"

#define DEFAULT_SPACING 2.0f
#define GRAB_SCALE 1.1f

#define ID_BIN -2 //negative id != -1 for special items

GlWidget::item_t::item_t(float pSize, texId_t pText)
    :x_(0),
     y_(0),
     z_(0),
     size_(pSize),
     sizeOffset_(0),
     yOffset_(0),
     texture_(pText),
     selected_(false),
     drawn_(false)
{
}

GlWidget::GlWidget(QWidget *parent) :
    Glview(60,parent),
    maxRecordTimer_(0),
    curentRecordTime_(0),
    recording_(false),
    playing_(false),
    selectionMode_(HandEvent::SINGLE),
    boxSize_(BOX_SIZE),
    gridSize_(0),
    spacing_(DEFAULT_SPACING),
    zoomOffset_(0),
    maxZoom_(BOX_SIZE),
    grabbing_(false)
{
    leapListener_.setReceiver(this);
    controller_.addListener(leapListener_);

    palmPos_.x = 0.0f;
    palmPos_.y = 0.0f;
    palmPos_.z = 5.0f;

    setCursor(Qt::BlankCursor);
    lineList_.clear();
}

GlWidget::~GlWidget()
{
    controller_.removeListener(leapListener_);
}

void GlWidget::initializeGL()
{
    loadTexture("../code/ressources/box.png", CRATE);
    loadTexture("../code/ressources/metal.jpg", METAL);
    writing_ = false;
    recordTimer_.restart();

    // Just display the drawing even without any drawing yet
    line_t line(Leap::Vector(0.0,0.0,0.0),Leap::Vector(0.0,0.0,0.0),0);
    lineList_.append(line);

    glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_MULTISAMPLE);
}

void GlWidget::resizeGL(int width, int height)
{
    if(height == 0)
        height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1.0f, -100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GlWidget::paintGL()
{
    // ============================
    // Render Scene
    // ============================
    // clear the back buffer and z buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // disable lighting
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //place the camera like the real head and look at the center
    Leap::Vector head = cam_.getPos();
    Leap::Vector focus = cam_.getFocus();
    gluLookAt(head.x,head.y,head.z, focus.x, focus.y, focus.z, 0.0f, 1.0f,0.0f);
    glClearColor(0,0,0,0);
    drawCube(CRATE, 0, 0, 0, 0.5);
    // Objects
    drawPalmPos();
    glLineWidth(5.5);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);


    if(recording_ or playing_)
    {
        curentRecordTime_ += recordTimer_.elapsed();
    }

    foreach (line_t line, lineList_) {
        if(line.timePainted_> curentRecordTime_)
        {
            continue;
        }

        line.firstPoint_ /= 10;
        line.secondPoint_ /= 10;

        glVertex3f(line.firstPoint_.x, line.firstPoint_.y-4, line.firstPoint_.z);
        glVertex3f(line.secondPoint_.x, line.secondPoint_.y-4, line.secondPoint_.z);
    }
    glEnd();
    if(maxRecordTimer_ < curentRecordTime_ + recordTimer_.elapsed())
    {
        maxRecordTimer_ = curentRecordTime_ + recordTimer_.elapsed();
    }
}

//helper function, loads a texture and assign it to an enum value
//to help retrieve it later
void GlWidget::loadTexture(QString textureName, texId_t pId)
{
    QImage qim_Texture;
    QImage qim_TempTexture;
    qim_TempTexture.load(textureName);
    qim_Texture = QGLWidget::convertToGLFormat( qim_TempTexture );
    glGenTextures( 1, &texture_[pId] );
    glBindTexture( GL_TEXTURE_2D, texture_[pId] );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, qim_Texture.width(), qim_Texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, qim_Texture.bits() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}




// record functions

void GlWidget::startRecord()
{
    recording_ = true;
    recordTimer_.restart();

}

void GlWidget::stopRecord()
{
    recording_ = false;
    playing_ = false;
}

void GlWidget::play()
{
    playing_ = true;
}

void GlWidget::setNewTime(int time)
{
    curentRecordTime_ = (time*maxRecordTimer_)/1000;
    recordTimer_.restart();
}


//Draw line

void GlWidget::drawLine(Leap::Vector firstPoint,Leap::Vector secondPoint)
{
    glLineWidth(5.5);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    char first[30];
    char second[30];
    sprintf(first,"first pos x : %f",firstPoint.x);
    sprintf(second,"second pos x : %f",secondPoint.x);

   // firstPoint /= 10;
   // secondPoint /= 10;
    qDebug() << first << endl << second;
    glVertex3f(firstPoint.x, firstPoint.y, firstPoint.z);
    glVertex3f(secondPoint.x, secondPoint.y, secondPoint.z);
    glEnd();

}

//Draw 6 squares and apply the texture on each: absolute coordinates for the center
void GlWidget::drawCube(texId_t PtextureId, float pCenterX, float pCenterY,float pCenterZ, float pSize)
{
    float half = pSize/2;
    glBindTexture(GL_TEXTURE_2D, texture_[PtextureId]);

    glBegin(GL_QUADS);
    // front fixed Z near (positive)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(pCenterX-half, pCenterY-half, pCenterZ-half);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(pCenterX+half, pCenterY-half, pCenterZ-half);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(pCenterX+half, pCenterY+half, pCenterZ-half);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(pCenterX-half, pCenterY+half, pCenterZ-half);

    // back fixed z far (negative)
    glTexCoord2f(1.0f, 0.0f); glVertex3f(pCenterX-half, pCenterY-half, pCenterZ+half);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(pCenterX-half, pCenterY+half, pCenterZ+half);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(pCenterX+half, pCenterY+half, pCenterZ+half);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(pCenterX+half, pCenterY-half, pCenterZ+half);

    // top fixed Y up
    glTexCoord2f(0.0f, 1.0f); glVertex3f(pCenterX-half,  pCenterY+half, pCenterZ-half);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(pCenterX-half,  pCenterY+half, pCenterZ+half);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(pCenterX+half,  pCenterY+half, pCenterZ+half);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(pCenterX+half,  pCenterY+half, pCenterZ-half);

    // bottom fixed Y down
    glTexCoord2f(1.0f, 1.0f); glVertex3f(pCenterX-half, pCenterY-half, pCenterZ-half);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(pCenterX+half, pCenterY-half, pCenterZ-half);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(pCenterX+half, pCenterY-half, pCenterZ+half);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(pCenterX-half, pCenterY-half, pCenterZ+half);

    // Right fixed X (positive)
    glTexCoord2f(1.0f, 0.0f); glVertex3f(pCenterX+half, pCenterY-half, pCenterZ-half);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(pCenterX+half, pCenterY+half, pCenterZ-half);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(pCenterX+half, pCenterY+half, pCenterZ+half);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(pCenterX+half, pCenterY-half, pCenterZ+half);

    // Left fixed x negative
    glTexCoord2f(0.0f, 0.0f); glVertex3f(pCenterX-half, pCenterY-half, pCenterZ-half);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(pCenterX-half, pCenterY-half, pCenterZ+half);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(pCenterX-half, pCenterY+half, pCenterZ+half);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(pCenterX-half, pCenterY+half, pCenterZ-half);
    glEnd();
}

//overloaded function for ease of use
void GlWidget::drawCube(const item_t& pCube)
{
    drawCube(pCube.texture_,
             pCube.x_,
             pCube.y_,
             pCube.z_,
             pCube.size_ + pCube.sizeOffset_);
}

//draw a cube where the middle of the palm is
void GlWidget::drawPalmPos()
{
    //normalize leap coordinates to our box size
   drawCube(METAL,
            palmPos_.x ,
            palmPos_.y,
            palmPos_.z, BOX_SIZE/(4*gridSize_));
}

/*
//find the closest cube from the palm center
int GlWidget::closestItem(float pTreshold)
{
    float minDist = 1000.0f;
    QList<item_t>::iterator it;
    int id = -1, i = 0;

    //search in the items displayed
    for (it = itemList_.begin(); it != itemList_.end(); it++)
    {
        Leap::Vector testV(it->x_,it->y_+it->yOffset_, it->z_);
        float delta = palmPos_.distanceTo(testV);
        if ( (delta <= pTreshold) && (delta <= minDist) )
        {
            minDist = delta;
            id = i;
        }
        i++;
    }

    //search in special items like bin or menu
    Leap::Vector testBin(bin_.x_, bin_.y_ + bin_.yOffset_, bin_.z_ );
    float delta = palmPos_.distanceTo(testBin);
    if ( (delta <= (bin_.size_ + bin_.sizeOffset_)) && (delta <= minDist) )
    {
        minDist = delta;
        id = ID_BIN;
    }
    return id;
}
*/

void GlWidget::customEvent(QEvent* pEvent)
{
    Leap::Vector fingerPos;
    HandEvent* event = dynamic_cast<HandEvent*>(pEvent);
    if ( event )
    {

        float offset = 0;
        //handle type of event
        switch (event->type() )
        {
        case HandEvent::Opened:
            //release nowhere, do nothing
            if ( grabbing_ )
            {
                grabbing_ = false;
                grabList_.clear();
            }
            break;
        case HandEvent::Closed:
            break;
        case HandEvent::Clicked:
            break;
        case HandEvent::DoubleClicked:
            break;
        case HandEvent::Zoom:
            offset = event->zoom();
            if ( zoomOffset_ + offset <= maxZoom_ )
                zoomOffset_ += offset;
            if ( zoomOffset_ < 0 )
                zoomOffset_ = 0;
            break;
        case HandEvent::Swiped:
            break;
        case HandEvent::Grabbed:
            break;
       case HandEvent::Moved:

            //convert normalize hand pos to our interaction box
            palmPos_ = (event->pos()+Vector(-0.5f,-0.5f,-1.0f))*boxSize_*1.5f;

            // Using finger pos
            fingerPos = leapListener_.getFingerPos();
            if(writing_ and recording_)
            {
                //drawLine(lastFingerPos,fingerPos);
                line_t line(lastFingerPos,fingerPos,curentRecordTime_ + recordTimer_.elapsed());
                lineList_.append(line);
            }
            lastFingerPos = fingerPos;
            break;
       case HandEvent::Circle:
            qDebug()<<"Circle Event!";
            break;
        default:
            break;
        }
    }
}

//update the camera position
void GlWidget::slotNewHead(head_t pPos)
{
    /*We inverse axes to compensate head position relative
     * to the cube.
     */

    Leap::Vector pos(pPos.x, pPos.y, pPos.z);
    cam_.slotUpdateFromHeadPos(pos);
    /*head_.x = -pPos.x;
    head_.y = -pPos.y;
    head_.z =  pPos.z;*/
}

//move slightly the camera, via keyboard commands for example
void GlWidget::slotMoveHead(int pAxis, float pDelta)
{
    Leap::Vector offset;
    switch(pAxis)
    {
        case 0:
            offset.x += pDelta;
            break;
        case 1:
            offset.y += pDelta;
            break;
        case 2:
            offset.z += pDelta;
        default:
            break;
    }
    cam_.slotTranslate(offset);
}
