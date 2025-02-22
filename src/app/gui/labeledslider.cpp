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

#include <cmath>
#include <cassert>

LabeledSlider::LabeledSlider(double minimum, double maximum,
              double interval, unsigned int precision,
              QWidget* parent)
    : QWidget{parent}
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
                          .relLenghtTickLine = 0.1,
                          .lineHeight = 0.1,
                          .labelAdditionalSpacing = 0.01,
                          .decreasingToRight = false,
                      }
                      , this};
    slider->setMinimum(std::round(minimum * std::pow(10, precision)));
    slider->setMaximum(std::round(maximum * std::pow(10, precision)));
    layout->addWidget(slider);
    layout->addWidget(labels);
}

