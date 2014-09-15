#include "mainwindow.h"
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include <QMenu>
#include <QDialog>

#include "CamSettingsWidget.h"

#define DELAY_FPS 50

mainwindow::mainwindow(QWidget *parent) :
    QMainWindow(parent),
    webcamView_(0),
    timer_(0),
    glView_(0)
{
}

 mainwindow::~mainwindow()
 {
 }

void mainwindow::init(void)
{
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0,0,0,0);

    webcamView_ = new QLabel("Face View",this);
    webcamView_->setWindowFlags(Qt::Window);
    webcamView_->setMinimumSize(500, 500);
    webcamView_->hide();

    tracker_.init();
    setWindowTitle("Linespace");
    QWidget::showMaximized();
    webcamView_->setBackgroundRole(QPalette::Base);
    webcamView_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    webcamView_->setScaledContents(true);
    timer_ = new QTimer(this);

    glView_ = new GlWidget(this);


    // View and slider layout
    QVBoxLayout* vlayout = new QVBoxLayout();

    vlayout->addWidget(glView_);
    hLayout->addItem(vlayout);

    QMenu* menu = menuBar()->addMenu("&Display");
    QAction* actionStart = new QAction("Start/Stop", this);
    QAction* actionHide = new QAction("Hide Webcam", this);
    QAction* actionSettings = new QAction("Change gains", this);
    menu->addAction(actionStart);
    menu->addAction(actionHide);
    menu->addAction(actionSettings);

    menu = menuBar()->addMenu("&Help");
    QAction* actionAbout = new QAction(QString("About ")+QCoreApplication::applicationName(),this);
    menu->addAction(actionAbout);

    connect(actionStart, SIGNAL(triggered()), this, SLOT(slotStart()));
    connect(actionHide, SIGNAL(triggered()), webcamView_, SLOT(hide()));
    connect(actionSettings, SIGNAL(triggered()), this, SLOT(slotCamSettings()));

    connect(actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));

    connect(timer_, SIGNAL(timeout()), this, SLOT(slotGetNewFrame()));
    connect(this,SIGNAL(signalNewFrame(QPixmap )), this, SLOT(slotUpdateFrame(QPixmap)));

    QWidget* centerWidget = new QWidget(this);
    centerWidget->setLayout(hLayout);
    setCentralWidget(centerWidget);

    connect(&tracker_, SIGNAL(signalNewHeadPos(head_t)), glView_, SLOT(slotNewHead(head_t)));
    glView_->setFocus();
}

void mainwindow::slotStart()
{
    if (timer_->isActive())
        timer_->stop();
    else
        timer_->start(DELAY_FPS);
}

QTime* mainwindow::setTimeFromInt(int intTime)
{
    QTime* time = new QTime();
    int hour = 0;
    int min = 0;
    int sec = 0;
    int msec = 0;
    while(true){
        if(intTime > 3600*1000)
        {
            hour += 1;
            intTime -= 3600*1000;
        }
        else if(intTime > 60*1000)
        {
            min += 1;
            intTime -= 60*1000;
        }
        else if(intTime > 1000)
        {
            sec += 1;
            intTime -= 1000;
        }
        else
        {
            msec = intTime;
            break;
        }

    }
    time->setHMS(hour,min,sec,msec);
    return time;
}

void mainwindow::slotGetNewFrame()
{
    tracker_.getNewImg();
    tracker_.detectHead();
    tracker_.drawFace();
    if (tracker_.isNewFace())
    {
        imgWebcam_ = tracker_.getPixmap();
        emit signalNewFrame(imgWebcam_);
    }
}

#define CAMERA_STEP 0.1
void mainwindow::keyPressEvent(QKeyEvent *keyEvent)
{
    switch(keyEvent->key())
    {
        case Qt::Key_Escape:
            if (isFullScreen())
            {
                showMaximized();
                menuBar()->show();
            }
            /*else{
                close();
                QApplication::quit();
            }*/
            break;
        case Qt::Key_F:
            if (isFullScreen())
            {
                showMaximized();
                menuBar()->show();
            }
            else
            {
                menuBar()->hide();
                showFullScreen();
            }
            break;
        case Qt::Key_Z:
            glView_->slotMoveHead(1, CAMERA_STEP);
            break;
        case Qt::Key_S:
            glView_->slotMoveHead(1, -CAMERA_STEP);
            break;
        case Qt::Key_Q:
            glView_->slotMoveHead(0, -CAMERA_STEP);
            break;
        case Qt::Key_D:
            glView_->slotMoveHead(0, CAMERA_STEP);
            break;
        case Qt::Key_A:
            glView_->slotMoveHead(2, -CAMERA_STEP);
            break;
        case Qt::Key_E:
            glView_->slotMoveHead(2, CAMERA_STEP);
            break;
        case Qt::Key_H:
            webcamView_->isHidden()? webcamView_->show() : webcamView_->hide();
            this->setFocus();
            break;
        case Qt::Key_W :
            if(glView_->writing_)
            {
                glView_->writing_ = false;
            }
            else
            {
                glView_->writing_ = true;
            }
            break;
        case Qt::Key_Space:
            slotStart();
            break;
    }
}


void mainwindow::slotUpdateFrame(QPixmap pNewFrame)
{
    //display webcam image in label,
    //only if it's visible to save processing time
    if (webcamView_->isVisible() )
        webcamView_->setPixmap(pNewFrame.scaled(webcamView_->width(), webcamView_->height(),Qt::KeepAspectRatio));
}

void mainwindow::slotAbout()
{
    QMessageBox about(this);
    about.setText(QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion()
                   + " Author: Loik Le Devehat");
    stringstream ss;
    ss<<"This application is distributed as free software."
        <<"It uses headtracking with opencv to simulate augmented reality\n"
        <<"and leapmotion to track your hands.\n"
        <<"Source code available on: <a href='https://github.com/loikled/linespace'> LoikLed Github repository</a>";
    about.setTextFormat(Qt::RichText);
    about.setInformativeText(QString::fromStdString(ss.str()));
    about.setIcon(QMessageBox::Information);
    about.adjustSize();
    about.exec();
}

void mainwindow::slotCamSettings(){

    CamSettingsWidget* settings = new CamSettingsWidget(this);
    settings->show();
    connect(settings, SIGNAL(signalChangeGains(Leap::Vector)), glView_, SLOT(slotChangeGains(Leap::Vector)));
}
