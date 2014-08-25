#include "mainwindow.h"
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include <QMenu>
#include <QDialog>

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
    hLayout->addWidget(webcamView_);
    webcamView_->hide();

    tracker_.init();
    setWindowTitle("Holotouch");
    QWidget::showMaximized();
    webcamView_->setBackgroundRole(QPalette::Base);
    webcamView_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    webcamView_->setScaledContents(true);
    timer_ = new QTimer(this);

    glView_ = new GlWidget(this);


    // View and slider layout
    QGridLayout* vlayout = new QGridLayout();
    slider_ = new QSlider();
    // to increase slider precision
    slider_->setMaximum(1000);
    connect(slider_, SIGNAL(sliderMoved(int)), this, SLOT(slotSliderMoved(int)));
    connect(slider_, SIGNAL(sliderReleased()), this, SLOT(slotRecord()));
    slider_->setOrientation(Qt::Horizontal);
    vlayout->addWidget(glView_);
    vlayout->addWidget(slider_);
    hLayout->addItem(vlayout);



    // Record and play drawing
    QPushButton* button = new QPushButton("Record");
    connect(button, SIGNAL(clicked()), this, SLOT(slotRecord()));
    QPushButton* button2 = new QPushButton("Stop");
    connect(button2, SIGNAL(clicked()), this, SLOT(slotStop()));
    QPushButton* button3 = new QPushButton("Play");
    connect(button3, SIGNAL(clicked()), this, SLOT(slotPlay()));
    button->setMaximumWidth(80);
    button2->setMaximumWidth(80);
    button3->setMaximumWidth(80);
    QLineEdit* textBlock = new QLineEdit("todo display recording timer");
    textBlock->setMaximumWidth(80);

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(button);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->addWidget(textBlock);

    hLayout->addItem(layout);




    QMenu* menu = menuBar()->addMenu("&Display");
    QAction* actionStart = new QAction("Start/Stop", this);
    QAction* actionHide = new QAction("Hide Webcam", this);
    menu->addAction(actionStart);
    menu->addAction(actionHide);

    menu = menuBar()->addMenu("&Help");
    QAction* actionAbout = new QAction(QString("About ")+QCoreApplication::applicationName(),this);
    menu->addAction(actionAbout);

    connect(actionStart, SIGNAL(triggered()), this, SLOT(slotStart()));
    connect(actionHide, SIGNAL(triggered()), webcamView_, SLOT(hide()));

    connect(actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));

    connect(timer_, SIGNAL(timeout()), this, SLOT(slotGetNewFrame()));
    connect(this,SIGNAL(signalNewFrame(QPixmap )), this, SLOT(slotUpdateFrame(QPixmap)));

    QWidget* centerWidget = new QWidget(this);
    centerWidget->setLayout(hLayout);
    setCentralWidget(centerWidget);

    connect(&tracker_, SIGNAL(signalNewHeadPos(head_t)), glView_, SLOT(slotNewHead(head_t)));



}

void mainwindow::slotSliderMoved(int value)
{
    glView_->setNewTime(value);
}

void mainwindow::slotRecord()
{
    glView_->startRecord();
}
void mainwindow::slotStop()
{
    glView_->stopRecord();
}
void mainwindow::slotPlay()
{
    glView_->play();
}

void mainwindow::slotStart()
{
    if (timer_->isActive())
        timer_->stop();
    else
        timer_->start(DELAY_FPS);
}

void mainwindow::slotGetNewFrame()
{
    tracker_.getNewImg();
    tracker_.detectHead();
    if (tracker_.isNewFace())
    {
        tracker_.drawFace();
        imgWebcam_ = tracker_.getPixmap();
        emit signalNewFrame(imgWebcam_);
    }
}

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
            else
                close();
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
            glView_->slotMoveHead(1, 0.05);
            break;
        case Qt::Key_S:
            glView_->slotMoveHead(1, -0.05);
            break;
        case Qt::Key_Q:
            glView_->slotMoveHead(0, -0.05);
            break;
        case Qt::Key_D:
            glView_->slotMoveHead(0, 0.05);
            break;
        case Qt::Key_A:
            glView_->slotMoveHead(2, -0.05);
            break;
        case Qt::Key_E:
            glView_->slotMoveHead(2, 0.05);
            break;
        case Qt::Key_H:
            webcamView_->isHidden()? webcamView_->show() : webcamView_->hide();
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
        <<"Source code available on: <a href='https://github.com/loikled/holotouch'> LoikLed Github repository</a>";
    about.setTextFormat(Qt::RichText);
    about.setInformativeText(QString::fromStdString(ss.str()));
    about.setIcon(QMessageBox::Information);
    about.adjustSize();
    about.exec();
}
