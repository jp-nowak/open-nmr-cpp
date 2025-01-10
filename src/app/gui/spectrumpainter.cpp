#include "spectrumpainter.h"

#include <algorithm>
#include <utility>

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

SpectrumPainter::SpectrumPainter(std::vector<std::complex<double>>* spectrum, QWidget* parent)
    : QWidget{parent}
    , spectrum{spectrum}
    , baselinePosition{0.125}
    , multiplier{1}
    , scalingFactor{1}

{
    spectrumPen.setCosmetic(true);
    spectrumPen.setWidth(1);
    spectrumPen.setColor(QColor(255, 0, 0));
    spectrumPen.setCapStyle(Qt::RoundCap);




}

void SpectrumPainter::zoom(QPointF startPos, QPointF endPos)
{
    double start = mapFromGlobal(startPos).x();
    double end = mapFromGlobal(endPos).x();


    if (start > end) {
        std::swap(start, end);
    }

    if ((start < 0) or (end > width())) {
        return;
    }

    size_t startPoint = std::ceil(start / width() * (spectrum.size() - 1));
    size_t endPoint = std::floor(end / width() * (spectrum.size() - 1));
    qDebug() << "zoom" << startPoint << endPoint;
    spectrum.setRange(startPoint, endPoint);
    update();

}

void SpectrumPainter::resetZoom()
{
    spectrum.reset();
    update();
}




void SpectrumPainter::paintEvent(QPaintEvent* e)
{


        QPainter painter(this);
        painter.drawPolygon(e->rect().adjusted(1,1,-1,-1));
        QPolygonF baseLine;
        baseLine << QPointF(0, e->rect().height() * (1 - baselinePosition))
                 << QPointF(e->rect().width(), e->rect().height() * (1 - baselinePosition));
        painter.drawPolyline(baseLine);

        double maximum = spectrum[spectrum.maxElemIndex].real();

        painter.setWindow(0, -maximum * multiplier, (spectrum.size() - 1) * multiplier, (maximum * (1/(1-baselinePosition))) * multiplier);
        painter.scale(1, -1);

        QPolygonF spectrumPolygon{};

        for (size_t i = 0; i < spectrum.size(); i++) {
            spectrumPolygon << QPointF(i * multiplier, spectrum[i].real() * multiplier * scalingFactor);
        }

        painter.setPen(spectrumPen);
        painter.drawPolyline(spectrumPolygon);
}

