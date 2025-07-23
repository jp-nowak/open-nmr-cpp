#include "labeledslider.h"

#include "xaxis.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QString>
#include <QFontMetrics>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSignalBlocker>

#include <cmath>

LabeledSlider::LabeledSlider(double minimum, double maximum,
              double interval, unsigned int precision, double initial,
              QWidget* parent)
    : QWidget{parent}
    , multiplier{std::pow(10, precision)}
    , layout{new QVBoxLayout{this}}
    , slider{new QSlider{Qt::Horizontal, this}}
{
    labels = new UniversalAxis{
                      AxisProperties{
                          .minimum = minimum,
                          .maximum = maximum,
                          .primaryTicksInterval = interval,
                          .fontSize = 10,
                          .showLine = true,
                          .displayPrecision = 0,
                          .vertical = false,
                          .descending = false,
                          .dynamic = false
                      }
                      , this};

    labels->setMinimumHeight(45);
    slider->setMinimum(std::round(minimum * multiplier));
    slider->setMaximum(std::round(maximum * multiplier));
    slider->setValue(initial * multiplier);
    slider->setFixedHeight(10);
    layout->addStretch(1);
    layout->addWidget(slider);
    layout->addWidget(labels);
    layout->addStretch(1);
    connect(slider, &QSlider::sliderMoved, this, &LabeledSlider::sliderMovedSlot);
}

std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> LabeledSlider::createWithBoxAndLabel(QString label, double minimum, double maximum,
                                                                    double interval, unsigned int precision, double initial, QWidget* parent)
{
    QWidget* mainWidget = new QWidget(parent);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    QHBoxLayout* subLayout = new QHBoxLayout(nullptr);
    QLabel* qlabel = new QLabel(label, mainWidget);
    QDoubleSpinBox* box = new QDoubleSpinBox(mainWidget);
    box->setMinimum(minimum);
    box->setMaximum(maximum);
    box->setDecimals(precision);
    box->setValue(initial);
    LabeledSlider* slider = new LabeledSlider(minimum, maximum, interval, precision, initial, mainWidget);
    connect(slider, &LabeledSlider::sliderMoved, [box](double i)
            {
                QSignalBlocker blocker(box);
                box->setValue(i);
            });
    connect(box, &QDoubleSpinBox::valueChanged, [slider](double i)
            {
                QSignalBlocker blocker(slider);
                slider->setValue(i);
            });
    subLayout->addWidget(qlabel);
    subLayout->addWidget(box);
    mainLayout->addLayout(subLayout);
    mainLayout->addWidget(slider);
    return {mainWidget, slider, box};
}

void LabeledSlider::changeValues(std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> i, double value)
{
    QSignalBlocker blocker1(std::get<LabeledSlider*>(i));
    QSignalBlocker blocker2(std::get<QDoubleSpinBox*>(i));
    std::get<LabeledSlider*>(i)->setValue(value);
    std::get<QDoubleSpinBox*>(i)->setValue(value);
}




void LabeledSlider::setValue(double value)
{
    if ((value * multiplier < slider->minimum()) or (value * multiplier > slider->maximum())) return;
    slider->setValue(value * multiplier);
}

void LabeledSlider::sliderMovedSlot(int value)
{
    emit sliderMoved(value / multiplier);
}

// SliderWithEditField::SliderWithEditField(double minimum, double maximum,
//                                          double interval, unsigned int precision,
//                                          double initial, QString label,
//                                          QWidget* parent)
//     : QWidget{parent}
//     , layout{new QHBoxLayout{this}}

// {
//     if (label.size()) {
//         this->label = new QLabel{label};
//         layout->addWidget(this->label);
//     }
//     slider = new LabeledSlider{minimum, maximum, interval, precision, initial, this};
//     layout->addWidget(slider);
//     box = new QDoubleSpinBox{this};
//     box->setRange(minimum, maximum);
//     box->setDecimals(2);
//     box->setValue(initial);
//     layout->addWidget(box);
//     connect(slider, &LabeledSlider::sliderMoved, this, &SliderWithEditField::valueChangedSlot);
//     connect(box, &QDoubleSpinBox::valueChanged, this, &SliderWithEditField::valueChangedSlot);
// }

// void SliderWithEditField::valueChangedSlot(double value)
// {
//     QSignalBlocker blocker{box};
//     box->setValue(value);
//     slider->setValue(value);
// }
