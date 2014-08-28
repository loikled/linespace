#ifndef CAMSETTINGSWIDGET_H
#define CAMSETTINGSWIDGET_H

#include "leapmotion/Leap.h"
#include <QSlider>
#include <QPushButton>

class CamSettingsWidget : public QWidget{
    Q_OBJECT
    private:
        Leap::Vector gains_;
        QSlider* sliderPhi_;
        QSlider* sliderTheta_;
        QSlider* sliderR_;
        QPushButton* btnOK_;

    public:
        CamSettingsWidget(QWidget* parent = 0);
        ~CamSettingsWidget();

    void initGui(void);

    public slots:
        void slotTheta(int theta);
        void slotPhi(int phi);
        void slotR(int r);

    signals:
        void signalChangeGains(Leap::Vector gains);
};

#endif // CAMSETTINGSWIDGET_H
