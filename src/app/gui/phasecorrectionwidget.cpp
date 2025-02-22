#include "phasecorrectionwidget.h"

#include <QVBoxLayout>

#include "labeledslider.h"

PhaseCorrectionWidget::PhaseCorrectionWidget(QWidget *parent)
    : QWidget{parent, Qt::Window}
{
    auto* labeledSlider = new LabeledSlider(0, 100, 10, 0, this);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(labeledSlider);
}
