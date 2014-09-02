#include "CamSettingsWidget.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

CamSettingsWidget::CamSettingsWidget(QWidget* parent)
    :QWidget(parent),
      gains_(1.0,1.0,1.0),
      sliderPhi_(0),
      sliderTheta_(0),
      sliderR_(0),
      editTheta_(0),
      editPhi_(0),
      editR_(0)
{
    initGui();
}

CamSettingsWidget::~CamSettingsWidget(){}

void CamSettingsWidget::initGui(){
    QGroupBox* groupBox = new QGroupBox("Camera gains");
    QGridLayout* glayout = new QGridLayout;
    sliderPhi_ = new QSlider();
    sliderTheta_ = new QSlider();
    sliderR_ = new QSlider();

    sliderPhi_->setOrientation(Qt::Horizontal);
    sliderTheta_->setOrientation(Qt::Horizontal);
    sliderR_->setOrientation(Qt::Horizontal);

    editTheta_ = new QDoubleSpinBox();
    editPhi_ = new QDoubleSpinBox();
    editR_ = new QDoubleSpinBox();

    glayout->addWidget(new QLabel("Horizontal angle"), 0, 0);
    glayout->addWidget(editTheta_, 0, 1);
    glayout->addWidget(sliderTheta_, 1, 0, 1,2);


    glayout->addWidget(new QLabel("Vertical angle"), 2, 0);
    glayout->addWidget(editPhi_, 2, 1);
    glayout->addWidget(sliderPhi_, 3, 0, 1,2);

    glayout->addWidget(new QLabel("Depth ratio"), 4, 0);
    glayout->addWidget(editR_, 4, 1);
    glayout->addWidget(sliderR_, 5, 0, 1,2);

    groupBox->setLayout(glayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();

    btnOK_ = new QPushButton("Ok");
    btnOK_->setFixedWidth(150);
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(btnOK_, 0, Qt::AlignRight);

    setLayout(mainLayout);
    setWindowFlags(Qt::Tool);
    connect(btnOK_, SIGNAL(clicked()), this, SLOT(close()));

    connect(sliderTheta_, SIGNAL(sliderMoved(int)), this, SLOT(slotTheta(int)));
    connect(sliderPhi_, SIGNAL(sliderMoved(int)), this, SLOT(slotPhi(int)));
    connect(sliderR_, SIGNAL(sliderMoved(int)), this, SLOT(slotR(int)));

    connect(editTheta_, SIGNAL(valueChanged(double)), this, SLOT(slotThetaSpin(double)));
    connect(editPhi_, SIGNAL(valueChanged(double)), this, SLOT(slotPhiSpin(double)));
    connect(editR_, SIGNAL(valueChanged(double)), this, SLOT(slotRSpin(double)));

    sliderPhi_->setMinimum(1);
    sliderPhi_->setMaximum(200);
    sliderPhi_->setValue(100);
    sliderTheta_->setMinimum(1);
    sliderTheta_->setMaximum(200);
    sliderTheta_->setValue(100);
    sliderR_->setMinimum(10);
    sliderR_->setMaximum(1000);
    sliderR_->setValue(100);

    editTheta_->setRange(0.1, 2.0);
    editTheta_->setSingleStep(0.1);
    editPhi_->setRange(0.1, 2.0);
    editPhi_->setSingleStep(0.1);
    editR_->setRange(0.1, 4.0);
    editR_->setSingleStep(0.1);

    slotUpdateSpinBox();
    slotUpdateSliders();
}

void CamSettingsWidget::slotUpdateSpinBox(){
    editTheta_->setValue(gains_.x);
    editPhi_->setValue(gains_.y);
    editR_->setValue(gains_.z);
}

//horizontal gain
void CamSettingsWidget::slotTheta(int theta){
    gains_.x = (float)theta/100;
    emit signalChangeGains(gains_);
    slotUpdateSpinBox();
}

//vertical gain
void CamSettingsWidget::slotPhi(int phi){
    gains_.y = (float)phi/100;
    emit signalChangeGains(gains_);
    slotUpdateSpinBox();
}

//depth gain
void CamSettingsWidget::slotR(int r){
    gains_.z = (float)r/100;
    emit signalChangeGains(gains_);
    slotUpdateSpinBox();
}

void CamSettingsWidget::slotUpdateSliders(){
    sliderTheta_->setValue(gains_.x*100);
    sliderPhi_->setValue(gains_.y*100);
    sliderR_->setValue(gains_.z*100);
}

//horizontal gain
void CamSettingsWidget::slotThetaSpin(double theta){
    gains_.x = (float)theta;
    emit signalChangeGains(gains_);
    slotUpdateSliders();
}

//vertical gain
void CamSettingsWidget::slotPhiSpin(double phi){
    gains_.y = (float)phi;
    emit signalChangeGains(gains_);
    slotUpdateSliders();
}

//depth gain
void CamSettingsWidget::slotRSpin(double r){
    gains_.z = (float)r;
    emit signalChangeGains(gains_);
    slotUpdateSliders();
}

