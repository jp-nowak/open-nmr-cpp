#include "spectrumdisplayer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>


SpectrumDisplayer::SpectrumDisplayer(SpectrumPainter* spainter, QWidget* parent)
    : QWidget{parent}
    , spainter{spainter}
{
        spainter->setParent(this);

        QVBoxLayout* spectrumAndXAxis = new QVBoxLayout();

        auto& info = spainter->experiment->info;


        xAxis = new XAxis{
            XAxisProperties{
                .left = info.plot_left_ppm,
                .right = info.plot_right_ppm,
                .primaryTicksInterval = 0.5,
                .secondaryTicksInterval = 0.25,
                .secTickProp = 0.25,
                .relLenghtTickLine = 0.15,
                .lineHeight = 0.3,
                .labelAdditionalSpacing = 0.25
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
