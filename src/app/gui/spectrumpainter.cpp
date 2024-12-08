#include "spectrumpainter.h"

#include <algorithm>

#include <QPainter>
#include <QPolygon>
#include <QPolygonF>

#include <QPoint>
#include <QPointF>
#include <QPen>

#include <QDebug>
#include <QPaintEvent>
#include <QList>
#include <QTransform>

SpectrumPainter::SpectrumPainter(std::unique_ptr<Spectrum>&& new_experiment, QWidget* parent)
    : QWidget{parent}
    , experiment{std::move(new_experiment)}
    , baselinePosition{0.125}
    , multiplier{1}
    , scalingFactor{1}
    , displayBegin{0}

{
    spectrumPen.setCosmetic(true);
    spectrumPen.setWidth(1);
    spectrumPen.setColor(QColor(255, 0, 0));
    spectrumPen.setCapStyle(Qt::RoundCap);

    const std::vector<std::complex<double>>& spectrum = experiment->get_spectrum();


    displayEnd = spectrum.size();
    maxSpectrumElemIndex = std::max_element(spectrum.begin(), spectrum.end(),
    [](const std::complex<double>& a, const std::complex<double>& b){return a.real() < b.real();})
    - spectrum.begin();

}

void SpectrumPainter::paintEvent(QPaintEvent* e)
{


        QPainter painter(this);
        painter.drawPolygon(e->rect().adjusted(1,1,-1,-1));
        QPolygonF baseLine;
        baseLine << QPointF(0, e->rect().height() * (1 - baselinePosition))
                 << QPointF(e->rect().width(), e->rect().height() * (1 - baselinePosition));
        painter.drawPolyline(baseLine);


        const std::vector<std::complex<double>>& spectrum = experiment->get_spectrum();

        double maximum = spectrum[maxSpectrumElemIndex].real();



        painter.setWindow(0, -maximum * multiplier, (spectrum.size() - 1) * multiplier, (maximum * (1/(1-baselinePosition))) * multiplier);
        painter.scale(1, -1);


        QPolygonF spectrumPolygon{};

        for (size_t i = displayBegin; (i < spectrum.size()) and (i < displayEnd); i++) {
            spectrumPolygon << QPointF(i * multiplier, spectrum[i].real() * multiplier * scalingFactor);
        }

        painter.setPen(spectrumPen);
        painter.drawPolyline(spectrumPolygon);
}

