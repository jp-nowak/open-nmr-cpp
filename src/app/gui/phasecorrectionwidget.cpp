#include "phasecorrectionwidget.h"

#include "labeledslider.h"
#include "../spectrum/spectrum.h"
#include "../processing/phase_correction.h"
#include "../mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScreen>
#include <QString>
#include <QLabel>

#include <tuple>

#define DEG_TO_RAD 2.0/360

PhaseCorrectionWidget::PhaseCorrectionWidget(Spectrum* experiment, QWidget *parent)
    : QWidget{parent, Qt::Window}
    , experiment{experiment}
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    ph0 = LabeledSlider::createWithBoxAndLabel(
        QStringLiteral(u"ph0"), -180, 180, 90, 1,
        experiment->getPhase().ph0.ph0, this);
    ph1 = LabeledSlider::createWithBoxAndLabel(
        QStringLiteral(u"ph1"), -360, 360, 180, 1,
        experiment->getPhase().ph1.ph1, this);
    pivot = LabeledSlider::createWithBoxAndLabel(
        QStringLiteral(u"pivot"), 0, 100, 25, 1,
        experiment->getPhase().ph1.pivot, this);

    mainLayout->addWidget(std::get<QWidget*>(ph0));
    mainLayout->addWidget(std::get<QWidget*>(ph1));
    mainLayout->addWidget(std::get<QWidget*>(pivot));
    setMinimumWidth(350);

    MainWindow* mainWindowPointer = MainWindow::findFrom(parent);

    connect(std::get<QDoubleSpinBox*>(ph0), &QDoubleSpinBox::valueChanged, this, &PhaseCorrectionWidget::ph0Slot);
    connect(std::get<LabeledSlider*>(ph0), &LabeledSlider::sliderMoved, this, &PhaseCorrectionWidget::ph0Slot);

    connect(std::get<QDoubleSpinBox*>(ph1), &QDoubleSpinBox::valueChanged, this, &PhaseCorrectionWidget::ph1Slot);
    connect(std::get<LabeledSlider*>(ph1), &LabeledSlider::sliderMoved, this, &PhaseCorrectionWidget::ph1Slot);

    connect(this, &PhaseCorrectionWidget::signalToRefreshDisplayedExperiment,
            mainWindowPointer, &MainWindow::refreshCurrentDisplayerSlot);

}

void PhaseCorrectionWidget::changeActiveExperiment(Spectrum* experiment)
{
    this->experiment = experiment;
    LabeledSlider::changeValues(ph0, experiment->getPhase().ph0.ph0);
    LabeledSlider::changeValues(ph1, experiment->getPhase().ph1.ph1);
    LabeledSlider::changeValues(pivot, experiment->getPhase().ph1.pivot);
}

void PhaseCorrectionWidget::ph0Slot(double phase)
{
    using namespace Processing;
    experiment->setPh0(Ph0(phase * DEG_TO_RAD));
    emit signalToRefreshDisplayedExperiment();
}

void PhaseCorrectionWidget::ph1Slot(double phase)
{
    using namespace Processing;
    experiment->setPh1(Ph1(phase * DEG_TO_RAD, std::get<QDoubleSpinBox*>(pivot)->value()));
    emit signalToRefreshDisplayedExperiment();
}
