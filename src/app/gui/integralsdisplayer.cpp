#include "integralsdisplayer.h"

#include "../spectrum/spectrum.h"
#include "gui_utilities.h"


#include <QPainter>
#include <QPaintEvent>

#include <utility>

IntegralsDisplayer::IntegralsDisplayer(const Spectrum* experiment, QWidget *parent)
    : QWidget{parent}
    , experiment{experiment}
    , startPoint_{0}
    , endPoint_{experiment->get_spectrum().size()}
    , currentSpectrumSize{endPoint_}
    , pen{}
{
    pen.setCosmetic(true);
    pen.setWidth(1);
    // pen.setColor(QColor(0, 255, 0));
    pen.setCapStyle(Qt::RoundCap);
}

void IntegralsDisplayer::setRange(size_t begin, size_t end)
{
    if (end < begin) {
        std::swap(begin, end);
    }

    if (end > experiment->get_spectrum().size()) return;

    startPoint_ = begin;
    endPoint_ = end;
}

void IntegralsDisplayer::recalculateDisplayRange()
{
    assert(currentSpectrumSize != 0);
    if (size_t n = experiment->get_spectrum().size(); n == currentSpectrumSize) {
        return;
    } else {
        startPoint_ = static_cast<double>(startPoint_) / currentSpectrumSize * n;
        endPoint_ = static_cast<double>(endPoint_) / currentSpectrumSize * n;
        currentSpectrumSize = n;
    }
}

void IntegralsDisplayer::paintEvent(QPaintEvent* e)
{
    recalculateDisplayRange();

    constexpr int multiplier = 100;
    constexpr double baselinePosition = 0.5;
    constexpr int displayPrecision = 2;

    QPainter painter(this);
    painter.setPen(pen);

    qDebug() << painter.viewTransformEnabled() << painter.worldMatrixEnabled();


    painter.setWindow(startPoint_ * multiplier, 0, (endPoint_ - 1) * multiplier, 100);

    qDebug() << painter.viewTransformEnabled() << painter.worldMatrixEnabled();


    // TRANFORMING WINDOW BREAKS DRAWING TEXT!!!

    QFont font = painter.font();
    font.setPixelSize(12);
    painter.setFont(font);

    qDebug() << "paint event";

    for (const auto& i : experiment->integrals) {

        if ((i.rightEdge <= startPoint_) or (i.rightEdge > endPoint_)) continue;

        QPolygonF baseLine;
        baseLine << QPointF(i.leftEdge * multiplier, e->rect().height() * (1 - baselinePosition))
                 << QPointF(i.rightEdge * multiplier, e->rect().height() * (1 - baselinePosition));
        painter.drawPolyline(baseLine);

        auto text = QString::number(i.relativeValue, 'f', displayPrecision);
        auto point = QPointF(i.leftEdge, e->rect().height() * (1 - baselinePosition));

        qDebug() << baseLine << i.relativeValue << text << point;

        painter.setPen(pen);

        painter.drawText(point, text);

        drawText(painter,
                 point,
                 Qt::AlignHCenter | Qt::AlignTop,
                 text);

    }
}
