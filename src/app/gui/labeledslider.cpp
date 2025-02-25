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
    labels = new XAxis{
                      XAxisProperties{
                          .left = minimum,
                          .right = maximum,
                          .primaryTicksInterval = interval,
                          .secondaryTicksInterval = 0.0,
                          .secTickProp = 0.25,
                          .relLenghtTickLine = 0.2,
                          .lineHeight = 0.5,
                          .labelAdditionalSpacing = 0.01,
                          .decreasingToRight = false,
                          .fontSize = 65,
                          .fixedDisplayPrecision = 0,
                          .showLine = true,
                      }
                      , this};
    labels->setFixedHeight(30);
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
