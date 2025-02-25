#include "phasecorrectionwidget.h"

#include "labeledslider.h"
#include "../spectrum/spectrum.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScreen>
#include <QString>
#include <QLabel>

#include <tuple>

PhaseCorrectionWidget::PhaseCorrectionWidget(Spectrum* experiment, QWidget *parent)
    : QWidget{parent, Qt::Window}
    , experiment{experiment}
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    ph0 = LabeledSlider::createWithBoxAndLabel(
        QStringLiteral(u"ph0"), -180, 180, 90, 1,
        experiment->getPhase().ph0.ph0, this);
    ph1 = LabeledSlider::createWithBoxAndLabel(
        QStringLiteral(u"ph1"), -180, 180, 90, 1,
        experiment->getPhase().ph1.ph1, this);
    pivot = LabeledSlider::createWithBoxAndLabel(
        QStringLiteral(u"pivot"), 0, 100, 25, 1,
        experiment->getPhase().ph1.pivot, this);

    mainLayout->addWidget(std::get<QWidget*>(ph0));
    mainLayout->addWidget(std::get<QWidget*>(ph1));
    mainLayout->addWidget(std::get<QWidget*>(pivot));
    setMinimumWidth(350);
}

void PhaseCorrectionWidget::changeActiveExperiment(Spectrum* experiment)
{
    this->experiment = experiment;
    LabeledSlider::changeValues(ph0, experiment->getPhase().ph0.ph0);
    LabeledSlider::changeValues(ph1, experiment->getPhase().ph1.ph1);
    LabeledSlider::changeValues(pivot, experiment->getPhase().ph1.pivot);
}

