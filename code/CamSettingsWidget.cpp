#include "CamSettingsWidget.h"
#include <QVBoxLayout>
#include <QGroupBox>

CamSettingsWidget::CamSettingsWidget(QWidget* parent)
    :QWidget(parent),
      sliderPhi_(0),
      sliderTheta_(0),
      sliderR_(0)
{
    initGui();
}

CamSettingsWidget::~CamSettingsWidget(){}

void CamSettingsWidget::initGui(){
    QGroupBox* groupBox = new QGroupBox("Camera gains");
    QVBoxLayout* vlayout = new QVBoxLayout();
    sliderPhi_ = new QSlider();
    sliderTheta_ = new QSlider();
    sliderR_ = new QSlider();

    vlayout->addWidget(sliderPhi_);
    vlayout->addWidget(sliderTheta_);
    vlayout->addWidget(sliderR_);

    groupBox->setLayout(vlayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();

    btnOK_ = new QPushButton("Ok");
    btnOK_->setFixedWidth(150);
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(btnOK_, 0, Qt::AlignRight);

    setLayout(mainLayout);
    setWindowFlags(Qt::Tool);
    connect(btnOK_, SIGNAL(clicked()), this, SLOT(close()));

    sliderPhi_->setMinimum(-500);
    sliderPhi_->setMaximum(+500);
    sliderPhi_->setValue(100);
    sliderTheta_->setMinimum(-500);
    sliderTheta_->setMaximum(+500);
    sliderTheta_->setValue(100);
    sliderR_->setMinimum(-500);
    sliderR_->setMaximum(+500);
    sliderR_->setValue(100);

    connect(sliderTheta_, SIGNAL(sliderMoved(int)), this, SLOT(slotTheta(int)));
    connect(sliderPhi_, SIGNAL(sliderMoved(int)), this, SLOT(slotPhi(int)));
    connect(sliderR_, SIGNAL(sliderMoved(int)), this, SLOT(slotR(int)));
}

void CamSettingsWidget::slotTheta(int theta){
    gains_.x = (float)theta/100;
    emit signalChangeGains(gains_);
}

void CamSettingsWidget::slotPhi(int phi){
    gains_.y = (float)phi/100;
    emit signalChangeGains(gains_);
}

void CamSettingsWidget::slotR(int r){
    gains_.z = (float)r/100;
    emit signalChangeGains(gains_);
}
