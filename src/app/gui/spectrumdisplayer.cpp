#include "spectrumdisplayer.h"

#include "../mainwindow.h"
#include "spectrumpainter.h"
#include "xaxis.h"
#include "integralsdisplayer.h"
#include "../processing/phase_correction.h"

#include <cassert>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QGridLayout>

namespace
{
QColor mapDisplayerActionToColor(DisplayerAction action)
{
    using enum DisplayerAction;
    switch(action)
    {
    case Zoom:
        return QColor(255, 0, 0, 100); // RED
    case Integrate:
        return QColor(0, 255, 0, 100); // GREEN
    case None:
        break;
    }
    return QColor(0, 0, 0, 100);
}
}

ASpectrumDisplayer::ASpectrumDisplayer(const SpectrumInfo& info, const FidSizeInfo& fidInfo, QWidget* parent)
: QWidget{parent}
, info{info}
, fidInfo{fidInfo}
{}

ASpectrumDisplayer::~ASpectrumDisplayer()
{

}

SpectrumDisplayer_1D::SpectrumDisplayer_1D(std::unique_ptr<Spectrum_1D>&& new_experiment, QWidget* parent)
: ASpectrumDisplayer{new_experiment->info, new_experiment->getFidSizeInfo(), parent}
, experiment{std::move(new_experiment)}
, spainter{new SpectrumPainter{experiment.get(), this}}
, idisplayer{new IntegralsDisplayer{experiment.get(), this}}
, mouseMoveStartPoint{0, 0}
, mainWindow{MainWindow::findFrom(this)}
{
    assert(mainWindow && "nullptr to main window");
    assert(parent && "nullptr to parent");

    // QVBoxLayout* spectrumAndXAxis = new QVBoxLayout();

    auto& info = experiment->info;

    xAxis = new UniversalAxis{AxisProperties{
                .minimum = info.plot_right_ppm,
                .maximum = info.plot_left_ppm,
                .fontSize = 15,
                .showLine = true,
                .vertical = false,
                .descending = true,
                .dynamic = true,
                .linePos = 0.1,
                .labelAdditionalSpacing = 0.01},
                              this};


    yAxis = new UniversalAxis{AxisProperties{
            .minimum = -(spainter->baselinePosition * spainter->maximum / (1 - spainter->baselinePosition)),
            .maximum = spainter->maximum,
            .fontSize = 15,
            .showLine = true,
            .vertical = true,
            .descending = true,
            .dynamic = true,
            .relTickLine = 0.1,
            .linePos = 0.1,
            .labelAdditionalSpacing = 0.1
                        }, this};
    // QLabel* yAxis = new QLabel(tr("Y"), this);
    // yAxis->setAlignment(Qt::AlignVCenter);

    // rescaling of yAxis when spainter is scrolled
    connect(spainter, &SpectrumPainter::wheelTurned, this, [this](double x){
        auto [maximum, minimum] = yAxis->getRange();
        if (x > 0) {
            yAxis->setRange(minimum / x, maximum / x);
        } else {
            x = std::fabs(x);
            yAxis->setRange(minimum * x, maximum * x);
        }
        yAxis->update();
    });

    QLabel* rightBottomEdge = new QLabel(tr("ppm"), this);
    rightBottomEdge->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    auto* layout = new QGridLayout();
    layout->addWidget(spainter, 0, 0);
    auto* xAxisLayout = new QVBoxLayout();
    xAxisLayout->addWidget(idisplayer);
    xAxisLayout->addWidget(xAxis);
    xAxisLayout->setStretchFactor(idisplayer, 1);
    xAxisLayout->setStretchFactor(xAxis, 2);
    layout->addLayout(xAxisLayout, 1, 0);
    layout->addWidget(yAxis, 0, 1);
    layout->addWidget(rightBottomEdge, 1, 1);
    layout->setColumnStretch(0, 15);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(0, 14);
    layout->setRowStretch(1, 2);
    setLayout(layout);

    // spectrumAndXAxis->addWidget(spainter);
    // spectrumAndXAxis->addWidget(idisplayer);
    // spectrumAndXAxis->addWidget(xAxis);
    // spectrumAndXAxis->setStretchFactor(spainter, 12);
    // spectrumAndXAxis->setStretchFactor(idisplayer, 1);
    // spectrumAndXAxis->setStretchFactor(xAxis, 1);





    // QHBoxLayout* spectrumWithXAxisAndYAxis = new QHBoxLayout();
    // spectrumWithXAxisAndYAxis->addLayout(spectrumAndXAxis);
    // spectrumWithXAxisAndYAxis->addWidget(yAxis);
    // spectrumWithXAxisAndYAxis->setStretchFactor(spectrumAndXAxis, 60);
    // spectrumWithXAxisAndYAxis->setStretchFactor(yAxis, 1);

    // setLayout(spectrumWithXAxisAndYAxis);

}

SpectrumDisplayer_1D::~SpectrumDisplayer_1D()
{

}

void SpectrumDisplayer_1D::mousePressEvent(QMouseEvent* e)
{
    mouseMoveStartPoint = e->pos();

    if (mainWindow->currentAction == DisplayerAction::None) return;

    spainter->setSelectionStart(mapToGlobal(mouseMoveStartPoint), mapDisplayerActionToColor(mainWindow->currentAction));
}

void SpectrumDisplayer_1D::mouseReleaseEvent(QMouseEvent* e)
{
    mouseMoveEndPoint = e->pos();

    using enum DisplayerAction;

    // double clicking on spectrum shall have no effect
    if (mouseMoveStartPoint.x() == mouseMoveEndPoint.x()) {
        mainWindow->finishAction();
        spainter->resetSelection();
        return;
    }

    switch (mainWindow->currentAction)
    {
    case Zoom:
        spainter->resetSelection();
        if (spainter->zoom(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint))) {
            xAxis->setRangePoints(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
            idisplayer->zoom(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
        }
        mainWindow->finishAction();
        break;

    case Integrate:
    {
        spainter->resetSelection();
        auto [left, right] = spainter->selectionRangeToDataPointsOfSpectrum(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
        experiment->integrate(left, right);
        mainWindow->finishAction();
        idisplayer->update();
        break;
    }
    case None:
        return;
    }
}

void SpectrumDisplayer_1D::mouseMoveEvent(QMouseEvent* e)
{
    if (mainWindow->currentAction == DisplayerAction::None) return;
    spainter->changeSelectionWidth(mapToGlobal(e->pos()), mapToGlobal(mouseMoveStartPoint));
}

void SpectrumDisplayer_1D::resetZoom()
{
    spainter->resetZoom();
    idisplayer->resetZoom();
    xAxis->setRange(experiment->info.plot_left_ppm, experiment->info.plot_right_ppm);
}

void SpectrumDisplayer_1D::updateAll()
{
    spainter->update();
    xAxis->update();
    idisplayer->update();
    update();
}


