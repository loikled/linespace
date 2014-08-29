#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "glwidget.h"
#include "head_tracking/facetrack.h"

class QLabel;
class QTimer;
class QMenu;

using namespace Leap;


class mainwindow : public QMainWindow
{
    Q_OBJECT

private:
    Facetrack tracker_;
    QLabel* webcamView_;
    QPixmap imgWebcam_;
    QTimer* timer_;
    GlWidget* glView_;
    QSlider* slider_;
    QLineEdit* textBlock;

public:
    mainwindow(QWidget *parent = 0);
    ~mainwindow();
    void init(void);
    void mainLoop(void);
    void keyPressEvent( QKeyEvent *keyEvent);

signals:
    void signalNewFrame(QPixmap pNewFrame);
    void signalNewHeadPos(head_t head);

public slots:
    void slotStart();
    void slotRecord();
    void slotStop();
    void slotPlay();
    void slotSliderMoved(int value);
    void slotSetTimings(int currentTime, int totalTime);

private slots:
   void slotGetNewFrame();
   void slotUpdateFrame(QPixmap pNewFrame);
   void slotAbout();
   void slotCamSettings();

};

#endif // MAINWINDOW_H
