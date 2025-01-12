#include "spectrumdisplayer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>

SpectrumDisplayer::SpectrumDisplayer(std::unique_ptr<Spectrum>&& new_experiment, QWidget* parent)
    : QWidget{parent}
    , currentAction{DisplayerAction::None}
    , experiment{std::move(new_experiment)}
    , spainter{new SpectrumPainter{&(experiment->spectrum)}}
    , mouseMoveStartPoint{0, 0}
{

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
        spectrumAndXAxis->addWidget(xAxis);
        spectrumAndXAxis->setStretchFactor(spainter, 12);
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

void SpectrumDisplayer::mousePressEvent(QMouseEvent* e)
{
    mouseMoveStartPoint = e->pos();
}

void SpectrumDisplayer::mouseReleaseEvent(QMouseEvent* e)
{
    mouseMoveEndPoint = e->pos();

    if (currentAction == DisplayerAction::Zoom)
    {
        qDebug() << "displayer zoom";
        spainter->zoom(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
        currentAction = DisplayerAction::None;
    }
}

void SpectrumDisplayer::resetZoom()
{
    spainter->resetZoom();
}
