#ifndef CAMSETTINGSWIDGET_H
#define CAMSETTINGSWIDGET_H

#include "leapmotion/Leap.h"
#include <QSlider>
#include <QPushButton>
#include <QDoubleSpinBox>

class CamSettingsWidget : public QWidget{
    Q_OBJECT
    private:
        Leap::Vector gains_;
        QSlider* sliderPhi_;
        QSlider* sliderTheta_;
        QSlider* sliderR_;
        QDoubleSpinBox* editTheta_;
        QDoubleSpinBox* editPhi_;
        QDoubleSpinBox* editR_;
        QPushButton* btnOK_;

    public:
        CamSettingsWidget(QWidget* parent = 0);
        ~CamSettingsWidget();

    void initGui(void);

    public slots:
        void slotTheta(int theta);
        void slotPhi(int phi);
        void slotR(int r);
        void slotUpdateSpinBox();
        void slotThetaSpin(double theta);
        void slotPhiSpin(double phi);
        void slotRSpin(double r);
        void slotUpdateSliders();

    signals:
        void signalChangeGains(Leap::Vector gains);
};

#endif // CAMSETTINGSWIDGET_H
