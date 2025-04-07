#include "spectrumdisplayer.h"

#include "../mainwindow.h"
#include "spectrumpainter.h"
#include "xaxis.h"
#include "integralsdisplayer.h"

#include <cassert>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>


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



SpectrumDisplayer::SpectrumDisplayer(std::unique_ptr<Spectrum>&& new_experiment, QWidget* parent)
    : QWidget{parent}
    , experiment{std::move(new_experiment)}
    , spainter{new SpectrumPainter{experiment.get(), this}}
    , idisplayer{new IntegralsDisplayer{experiment.get(), this}}
    , mouseMoveStartPoint{0, 0}
    , mainWindow{MainWindow::findFrom(this)}

{
    assert(mainWindow && "nullptr to main window");


    QVBoxLayout* spectrumAndXAxis = new QVBoxLayout();

    auto& info = experiment->info;

    xAxis = new XAxis{
        XAxisProperties{
            .left = info.plot_left_ppm,
            .right = info.plot_right_ppm,
            // .left = 0.01,
            // .right = 0.003,
            .primaryTicksInterval = 0.0,
            .secondaryTicksInterval = 0.0,
            .secTickProp = 0.25,
            .relLenghtTickLine = 0.5,
            .lineHeight = 0.5,
            .labelAdditionalSpacing = 0.01
        }
        , this};

    spectrumAndXAxis->addWidget(spainter);
    spectrumAndXAxis->addWidget(idisplayer);
    spectrumAndXAxis->addWidget(xAxis);
    spectrumAndXAxis->setStretchFactor(spainter, 12);
    spectrumAndXAxis->setStretchFactor(idisplayer, 1);
    spectrumAndXAxis->setStretchFactor(xAxis, 1);

    QLabel* yAxis = new QLabel(tr("Y"), this);
    yAxis->setAlignment(Qt::AlignVCenter);


    QHBoxLayout* spectrumWithXAxisAndYAxis = new QHBoxLayout();
    spectrumWithXAxisAndYAxis->addLayout(spectrumAndXAxis);
    spectrumWithXAxisAndYAxis->addWidget(yAxis);
    spectrumWithXAxisAndYAxis->setStretchFactor(spectrumAndXAxis, 60);
    spectrumWithXAxisAndYAxis->setStretchFactor(yAxis, 1);

    setLayout(spectrumWithXAxisAndYAxis);

}

SpectrumDisplayer::~SpectrumDisplayer()
{

}

void SpectrumDisplayer::mousePressEvent(QMouseEvent* e)
{
    mouseMoveStartPoint = e->pos();

    if (mainWindow->currentAction == DisplayerAction::None) return;

    spainter->setSelectionStart(mapToGlobal(mouseMoveStartPoint), mapDisplayerActionToColor(mainWindow->currentAction));
}

void SpectrumDisplayer::mouseReleaseEvent(QMouseEvent* e)
{
    mouseMoveEndPoint = e->pos();

    using enum DisplayerAction;

    // double clicking on spectrum shall have no effect
    if (mouseMoveStartPoint.x() == mouseMoveEndPoint.x()) {
        mainWindow->setCurrentAction(None);
        spainter->resetSelection();
        return;
    }

    switch (mainWindow->currentAction)
    {
    case Zoom:
        spainter->resetSelection();
        if (spainter->zoom(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint))) {
            xAxis->setRangePoints(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
        }
        mainWindow->setCurrentAction(DisplayerAction::None);
        break;

    case Integrate:
    {
        spainter->resetSelection();
        auto [left, right] = spainter->selectionRangeToDataPointsOfSpectrum(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
        experiment->integrate(left, right);
        mainWindow->setCurrentAction(DisplayerAction::None);
        idisplayer->update();
        break;
    }
    case None:
        return;
    }
}

void SpectrumDisplayer::mouseMoveEvent(QMouseEvent* e)
{
    if (mainWindow->currentAction == DisplayerAction::None) return;
    spainter->changeSelectionWidth(mapToGlobal(e->pos()), mapToGlobal(mouseMoveStartPoint));
}


void SpectrumDisplayer::resetZoom()
{
    spainter->resetZoom();
    xAxis->setRange(experiment->info.plot_left_ppm, experiment->info.plot_right_ppm);
}

void SpectrumDisplayer::updateAll()
{
    spainter->update();
    xAxis->update();
    update();
}


