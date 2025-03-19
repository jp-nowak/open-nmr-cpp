#include "widgets_for_processing.h"

#include "labeledslider.h"
#include "../spectrum/spectrum.h"
#include "../processing/phase_correction.h"
#include "../mainwindow.h"
#include "../processing/zero_filling.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScreen>
#include <QString>
#include <QLabel>
#include <QListWidget>
#include <QStringList>

#include <tuple>
#include <algorithm>

#define DEG_TO_RAD 2.0/360.0
#define RAD_TO_DEG 360.0/2.0

//------------------------------------------------------------------------------------------------------------------------

ProcessingWidget::ProcessingWidget(Spectrum* experiment, QWidget *parent)
: QWidget{parent}
, experiment{experiment}
, mainWindowPointer{MainWindow::findFrom(parent)}
{
    connect(this, &PhaseCorrectionWidget::signalToRefreshDisplayedExperiment,
            mainWindowPointer, &MainWindow::refreshCurrentDisplayerSlot);
    connect(mainWindowPointer, &MainWindow::displayedSpectrumChanged, this, &ProcessingWidget::changeActiveExperiment);
}

//------------------------------------------------------------------------------------------------------------------------

PhaseCorrectionWidget::PhaseCorrectionWidget(Spectrum* experiment, QWidget *parent)
    : ProcessingWidget{experiment, parent}
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

    connect(std::get<QDoubleSpinBox*>(ph0), &QDoubleSpinBox::valueChanged, this, &PhaseCorrectionWidget::ph0Slot);
    connect(std::get<LabeledSlider*>(ph0), &LabeledSlider::sliderMoved, this, &PhaseCorrectionWidget::ph0Slot);

    connect(std::get<QDoubleSpinBox*>(ph1), &QDoubleSpinBox::valueChanged, this, &PhaseCorrectionWidget::ph1Slot);
    connect(std::get<LabeledSlider*>(ph1), &LabeledSlider::sliderMoved, this, &PhaseCorrectionWidget::ph1Slot);

    PhaseCorrectionWidget::changeActiveExperiment(experiment);
}

void PhaseCorrectionWidget::changeActiveExperiment(Spectrum* experiment)
{
    this->experiment = experiment;
    LabeledSlider::changeValues(ph0, experiment->getPhase().ph0.ph0 * RAD_TO_DEG);
    LabeledSlider::changeValues(ph1, experiment->getPhase().ph1.ph1 * RAD_TO_DEG);
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

//------------------------------------------------------------------------------------------------------------------------

ZeroFillingWidget::ZeroFillingWidget(Spectrum* experiment, QWidget *parent)
    : ProcessingWidget{experiment, parent}
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    list = new QListWidget(this);

    QStringList labels{};

    for (auto i : Processing::POWERS_OF_TWO) {
        labels << QString::number(i);
    }

    list->addItems(labels);
    layout->addWidget(list);

    connect(list, &QListWidget::currentRowChanged, this, &ZeroFillingWidget::zeroFillingSlot);

    ZeroFillingWidget::changeActiveExperiment(experiment);
}

void ZeroFillingWidget::changeActiveExperiment(Spectrum* experiment)
{
    using namespace Processing;
    this->experiment = experiment;
    list->setCurrentRow(std::find(
                        POWERS_OF_TWO.begin(), POWERS_OF_TWO.end(), experiment->get_spectrum().size())
                        - POWERS_OF_TWO.begin());
}

void ZeroFillingWidget::zeroFillingSlot(int n)
{
    experiment->zeroFillOrTruncate(Processing::POWERS_OF_TWO[n]);
    emit signalToRefreshDisplayedExperiment();
}

