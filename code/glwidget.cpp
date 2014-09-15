#include "glwidget.h"

#include <math.h>
#ifdef __APPLE__
    #include <OPENGL/gl.h>
    #include <OPENGL/glu.h>
#else
    #include <GL/glu.h>
#endif
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
    Glview(100, parent),
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

    // Just display the drawing even without any drawing yet

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING); //enable lighting in the scene
    glEnable(GL_LIGHT0); //turn on light 0
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);
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

    // clear the color buffer(background in white) and z buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor (1.0, 1.0, 1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_LINE_SMOOTH);
    glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

    //place the camera like the real head and look at the center
    Leap::Vector head = cam_.getPos();
    Leap::Vector focus = cam_.getFocus();

    gluLookAt(head.x, head.y, head.z, focus.x, focus.y, focus.z, 0.0f, 1.0f,0.0f);

    //draw the grid to better show 3D scene
    drawGrid();

    // Objects
    drawCurve(shape_);
    drawCursor();
    drawFocus();
    drawShapes();
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

void GlWidget::drawCurve(const Shape shape, float r, float g,  float b, float a){

    glDisable(GL_LIGHTING);
    glColor4f(r,g,b,a);
    glLineWidth(4);
    glBegin(GL_LINES);

    foreach (Shape::line_t line, shape.getList()) {
        glVertex3f(line.firstPoint_.x, line.firstPoint_.y, line.firstPoint_.z);
        glVertex3f(line.secondPoint_.x, line.secondPoint_.y, line.secondPoint_.z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void GlWidget::drawShapes(){
    int highlight = closestShapeIndex();
    for(int i=0; i < shapeList_.size(); i++){
        if (highlight == i)
        {
            drawCurve(shapeList_[i], 0,1,0,1); //highlight color is green
        }else{
            drawCurve(shapeList_[i]);
        }
    }

}

void GlWidget::drawGrid(){
    const Grid::GridList_t* list = grid_.getLineList();
    glDisable(GL_LIGHTING);
    glLineWidth(2.0);
    glColor4f(1.0, 0.0, 0.0, 0.5);
    glBegin(GL_LINES);
    for(auto& line: *list){
        Leap::Vector first = line.first;
        Leap::Vector second = line.second;
        glNormal3f(0,0,1);
        glVertex3f(first.x, first.y, first.z);
        glVertex3f(second.x, second.y, second.z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

//Draw 6 squares and apply the texture on each: absolute coordinates for the center
void GlWidget::drawCube(texId_t PtextureId, float pCenterX, float pCenterY,float pCenterZ, float pSize)
{
    float half = pSize/2;
    glBindTexture(GL_TEXTURE_2D, texture_[PtextureId]);

    glBegin(GL_QUADS);
    // front fixed Z near (positive)

    glNormal3f(0,0,1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(pCenterX-half, pCenterY-half, pCenterZ-half);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(pCenterX+half, pCenterY-half, pCenterZ-half);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(pCenterX+half, pCenterY+half, pCenterZ-half);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(pCenterX-half, pCenterY+half, pCenterZ-half);

    // back fixed z far (negative)
    glNormal3f(0,0,1);
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

void GlWidget::drawCursor(){
    glDisable(GL_LIGHTING);

    const GLfloat* color = cursor_.getColorFromState();
    glColor4fv(color);
    Leap::Vector pos = cursor_.getPos();
    float size = cursor_.getSize();
    float lineSize = 4*size;
    Cursor::CursorMode_e mode = cursor_.getMode();

    GLUquadric* quad = gluNewQuadric();//for drawing a sphere
    Shape circle;
    Shape line;
    switch(mode){
        case Cursor::CURVE:
            gluQuadricOrientation(quad, GLU_OUTSIDE);
            //glusphere draws always at 0,0 so we change the model draw space
            glPushMatrix();
            glTranslatef(pos.x, pos.y, pos.z);
            gluSphere(quad, size , 100, 100);
            glPopMatrix();
            break;

        case Cursor::CIRCLE:
            circle.newType(Shape::CIRCLE);
            circle.changeCircleCenter(pos);
            circle.changeCircleSize(size);
            drawCurve(circle);
            break;

        case Cursor::SEGMENT:
             line.newType(Shape::LINE);
             line.changeLeft(Leap::Vector(pos.x + lineSize, pos.y + lineSize, pos.z + lineSize));
             line.changeRight(Leap::Vector(pos.x - lineSize, pos.y - lineSize, pos.z - lineSize));
             drawCurve(line);
             break;
    default:
            break;
    }

    gluDeleteQuadric(quad);

    Grid::GridList_t markers = grid_.getMarkers(pos);
    glLineWidth(2.0);
    glColor4f(1.0, 1.0, 1.0, 0.5);
    glBegin(GL_LINES);
    for(auto& line: markers){
        Leap::Vector first = line.first;
        Leap::Vector second = line.second;
        glVertex3f(first.x, first.y, first.z);
        glVertex3f(second.x, second.y, second.z);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void GlWidget::drawFocus(){
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat light_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricOrientation(quad, GLU_OUTSIDE);

    Leap::Vector focus = cam_.getFocus();

    float size = 0.02f;
    //glusphere draws always at 0,0 so we change the model draw space
    glPushMatrix();
    glTranslatef(focus.x, focus.y, focus.z);
    gluSphere(quad, size , 100, 100);
    glPopMatrix();
    gluDeleteQuadric(quad);
}

void GlWidget::clearCurves(){
    shapeList_.clear();
}

void GlWidget::updateShape(){
    Cursor::CursorMode_e mode = cursor_.getMode();
    int state = cursor_.getState();

    switch(mode){
        case Cursor::CURVE:
            switch(state){
                case Cursor::IDLE:
                        shape_.newType(Shape::CURVE);
                    break;
                case Cursor::STATE1:
                        shape_.addLine(cursor_.getLastMove());
                    break;
                case Cursor::STATE2:
                    cursor_.changeState(Cursor::IDLE);
                    shapeList_.append(shape_);
                    break;
                default:
                    break;
            }
            break;
        case Cursor::SEGMENT:
            switch(state){
                case Cursor::STATE1:
                    break;
                case Cursor::STATE2:
                    break;
                default:
                    break;
            }
            break;
        case Cursor::CIRCLE:
            switch(state){
                case Cursor::IDLE :


                    break;
                case Cursor::STATE1:
                    break;
                case Cursor::STATE2:
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

int GlWidget::closestShapeIndex(float treshold){
    Leap::Vector pos = cursor_.getPos();
    float min = treshold; //must be close enough to something
    int index = -1;
    for(int i = 0; i < shapeList_.size(); ++i){

        Shape s = shapeList_[i];
        for(Shape::line_t l : s.getList()){
            float dist = pos.distanceTo(l.firstPoint_);
            if (dist < min){
                min = dist;
                index = i;
            }
        }
    }
    return index;
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
                cursor_.slotNextMode();
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
                clearCurves();
                cam_.resetFocus();
                break;
        case HandEvent::Grabbed:
            break;
       case HandEvent::Moved:
            //update cursor to our coordinates
            cursor_.slotMove(event->pos());
            if(writing_ != event->writing())
            {
                cursor_.changeState(cursor_.getState()+1);

            }
            writing_ = event->writing();

            break;

       case HandEvent::Circle:

            break;

       case HandEvent::Slider:
/*
            if(abs(event->sliderAngle()) < 55)
            {
                newCurrentRecordTime = curentRecordTime_ - (int)event->sliderAngle()/10;
            }
            else
            {
                newCurrentRecordTime = curentRecordTime_ - (int)event->sliderAngle();
            }

            if(newCurrentRecordTime > 0 && newCurrentRecordTime < maxRecordTimer_)
            {
                curentRecordTime_ = newCurrentRecordTime;
            }
            */
/*
            if(abs(event->sliderAngle()) < 40)
            {
                float newDist = event->sliderAngle()/10;
                newDist = -newDist; //change orientation

                int index = cam_.getFocusIndex();
                Leap::Vector focus = cam_.getFocus();
                float dist = 0;

                if (index < lineList_.size()){
                    Shape::line_t line = lineList_.at(index);
                    if (newDist < 0){
                        dist = -focus.distanceTo(line.firstPoint_);
                        if (newDist < dist){
                            focus = line.firstPoint_;
                            index--;
                            if (index < 0)
                                index = 0;
                        }else{
                            focus = focus + (focus - line.firstPoint_)*dist;
                        }
                    }
                    else{
                        dist = focus.distanceTo(line.secondPoint_);
                        if (newDist > dist){
                            focus = line.secondPoint_;
                            index++;
                            if (index >= lineList_.size())
                                index = lineList_.size()-1;
                        }else{
                            focus = focus + (focus - line.secondPoint_)*dist;
                        }
                    }
                    cam_.setFocus(focus);
                    cam_.setFocusIndex(index);
                }

            }*/
            break;
        case HandEvent::Pinch:
            if(event->pinchIsLeft())
            {
                cursor_.isPinchLeft_  = true;
                cursor_.leftPinchPosition_   = cursor_.getPos(event->pos());
            }
            else
            {
                cursor_.isPinchRight_ = true;
                cursor_.rightPinchPosition_ = cursor_.getPos(event->pos());
            }

            break;
        case HandEvent::Release:
            if(event->pinchIsLeft())
            {
                cursor_.isPinchLeft_  = false;
            }
            else
            {
                cursor_.isPinchRight_ = false;
            }
            break;
    default:
            break;
        }
    }

    updateShape();
}

//update the camera position
void GlWidget::slotNewHead(head_t pPos)
{
    /*We inverse axes to compensate head position relative
     * to the cube.
     */

    Leap::Vector pos(pPos.x, pPos.y, pPos.z);
    cam_.slotUpdateFromHeadPos(pos);
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

void GlWidget::slotChangeGains(Leap::Vector gains)
{
    cam_.slotChangeFactor(gains);
}
