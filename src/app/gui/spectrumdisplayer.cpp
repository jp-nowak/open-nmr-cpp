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
        QLabel* xAxis = new QLabel(tr("x axis to be done"), this);
        xAxis->setAlignment(Qt::AlignHCenter);

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
