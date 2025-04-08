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
    pen.setWidth(2);
    pen.setColor(QColor(0, 255, 0));
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

    QPainter painter{this};

    const double startPoint = startPoint_; // TODO create uniform struct with setting for IntegralsDisplayer and XAxis
    const double endPoint = endPoint_;
    const double width = e->rect().width();
    const double height = e->rect().height();
    constexpr double baselineHeight = 0.5;
    constexpr double displayPrecision = 2;
    constexpr double tickHeight = 0.1;

    auto xPos = [startPoint, endPoint, width](double x){
        return x / (endPoint - startPoint) * width;
    };


    for (const auto& i : experiment->integrals) {

        painter.setPen(pen);

        if ((i.rightEdge <= startPoint_) or (i.rightEdge > endPoint_)) continue;

        { // TODO to be separated into function
        QPolygonF line;
        line << QPointF(xPos(i.leftEdge), height * (1 - baselineHeight))
             << QPointF(xPos(i.rightEdge), height * (1 - baselineHeight));
        painter.drawPolyline(line);
        }

        {
        QPolygonF line;
        line << QPointF(xPos(i.leftEdge), height * (1 - baselineHeight - tickHeight))
             << QPointF(xPos(i.leftEdge), height * (1 - baselineHeight + tickHeight));
        painter.drawPolyline(line);
        }

        {
        QPolygonF line;
        line << QPointF(xPos(i.rightEdge), height * (1 - baselineHeight - tickHeight))
             << QPointF(xPos(i.rightEdge), height * (1 - baselineHeight + tickHeight));
        painter.drawPolyline(line);
        }

        auto text = QString::number(i.relativeValue, 'f', displayPrecision);
        auto point = QPointF(xPos(i.leftEdge + (i.rightEdge - i.leftEdge) / 2), e->rect().height() * (1 - baselineHeight + tickHeight));

        painter.setPen(QColor("black"));

        drawText(painter,
                 point,
                 Qt::AlignHCenter | Qt::AlignTop,
                 text);
    }

}
