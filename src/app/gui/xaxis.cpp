#include "xaxis.h"
#include "gui_utilities.h"

#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QPointF>
#include <QPaintEvent>
#include <QDebug>
#include <QFont>
#include <QRectF>
#include <QString>

#include <cmath>
#include <cassert>

namespace {

    // symetric
    double findInterval(double span)
    {
        auto inRange = [span](double low, double high){return !(span < low) && (span < high);};

        if (inRange(5, 1000)) {

            if (inRange(5, 10)) return 1;
            if (inRange(10, 20)) return 2;
            if (inRange(20, 30)) return 3;
            if (inRange(30, 40)) return 4;
            if (inRange(40, 50)) return 5;
            if (inRange(50, 60)) return 6;
            if (inRange(60, 70)) return 7;
            if (inRange(70, 80)) return 8;
            if (inRange(80, 90)) return 9;
            if (inRange(90, 150)) return 10;
            if (inRange(150, 250)) return 20;
            if (inRange(250, 350)) return 30;
            if (inRange(350, 450)) return 40;
            if (inRange(450, 550)) return 50;
            if (inRange(550, 650)) return 60;
            if (inRange(650, 1000)) return 100;
        }

        if (inRange(1, 5)) {
            if (inRange(1, 2)) return 0.2;
            if (inRange(2, 3)) return 0.3;
            if (inRange(3, 4)) return 0.4;
            if (inRange(4, 5)) return 0.5;
        }

        if (inRange(0.01, 1)) {
            if (inRange(0.01, 0.1)) return 0.01;
            if (inRange(0.10, 0.20)) return 0.02;
            if (inRange(0.20, 0.30)) return 0.03;
            if (inRange(0.30, 0.40)) return 0.04;
            if (inRange(0.40, 0.50)) return 0.05;
            if (inRange(0.50, 0.60)) return 0.06;
            if (inRange(0.60, 0.70)) return 0.07;
            if (inRange(0.70, 0.80)) return 0.08;
            if (inRange(0.80, 0.90)) return 0.09;
            if (inRange(0.90, 1.00)) return 0.10;
        }

        return [](double x){
            int power10 = std::floor(std::log10(x));
            if (power10 < 0)
            {
                x *= std::pow(10, -power10);
            }
            int interval = std::ceil(x);
            double result = interval / std::pow(10, std::abs(std::ceil(std::log10(interval))));

            int power10Result = std::floor(std::log10(result));
            if (power10Result < 0)
            {
                result *= std::pow(10, -power10Result);
            }
            result = std::round(result);
            if (power10Result < 0)
            {
                result /= std::pow(10, -power10Result);
            }

            return (power10 < 0) ? result / std::pow(10, -power10) : result;
        }(span);
    }

    int calcDisplayPrecision(double x)
    {
        double divisor = 1;
        int precision = 1;
        for (; x < divisor; precision++, divisor /=10) {continue;} // empty body
        return precision;
    }

    double findFirstDivisibleNumber(double x, double y, bool decreasing = true)
    {
        int r = (decreasing) ? 1 : -1;
        if (x == 0.0) {return x;}
        int xpower10 = std::floor(std::log10(std::fabs(x)));
        int ypower10 = std::floor(std::log10(std::fabs(y)));
        int multiplier = 1;
        if (xpower10 < 0)
        {
            multiplier = std::pow(10, std::abs(xpower10));
        }
        if (ypower10 < 0)
        {
            multiplier *= std::pow(10, std::abs(ypower10));
        }
        int leftInteger = std::floor(x * multiplier);
        int tickInterval = y * multiplier;
        int firstPosition = leftInteger;
        for (; firstPosition % tickInterval; firstPosition -= r) {continue;} // empty body
        return static_cast<double>(firstPosition) / multiplier;
    }

}


UniversalAxis::UniversalAxis(AxisProperties properties, QWidget* parent)
    : p{properties}
{
    assert(parent && "nullptr to parent");
    initialize();
}

void UniversalAxis::initialize()
{
    assert(p.minimum < p.maximum);
    if (p.dynamic) {
        p.primaryTicksInterval = findInterval(p.maximum - p.minimum);
        p.secondaryTicksInterval = p.primaryTicksInterval / p.primaryToSecondaryTickRatio;
        p.displayPrecision = calcDisplayPrecision(p.primaryTicksInterval);
    }
}

double UniversalAxis::xPos(double x)
{
    const double valueWidth = p.maximum - p.minimum;
    if (p.vertical) {
        if (p.descending) {
            return (1 - ((x - p.minimum) / valueWidth)) * height();
        } else {
            return ((x - p.minimum) / valueWidth) * height();
        }
    } else {
        if (p.descending) {
            return (1 - ((x - p.minimum) / valueWidth)) * width();
        } else {
            return ((x - p.minimum) / valueWidth) * width();
        }
    }
}

void UniversalAxis::setRange(double minimum, double maximum)
{
    if (not p.dynamic) return;
    if (minimum > maximum) std::swap(minimum, maximum);
    p.maximum = maximum;
    p.minimum = minimum;
    initialize();
    update();
}
void UniversalAxis::setRangePoints(QPointF start, QPointF end)
{
    if (not p.dynamic) return;

    start = mapFromGlobal(start);
    end = mapFromGlobal(end);

    double startCoord = (p.vertical) ? start.y() : start.x();
    double endCoord = (p.vertical) ? end.y() : end.x();
    // start shall be point on left or on the top
    if (startCoord > endCoord) std::swap(startCoord, endCoord);
    const double dimension = (p.vertical) ? height() : width();

    if (startCoord <= 0.0) { // selection reaches left (top) edge of widget
        startCoord = (p.descending) ? p.maximum : p.minimum;
    } else {
        startCoord = (p.descending) ? (1 - static_cast<double>(startCoord) / dimension)
                                       * (p.maximum - p.minimum) + p.minimum
                                    : (static_cast<double>(startCoord) / dimension)
                                       * (p.maximum - p.minimum) + p.minimum;
    }
    if (endCoord >= dimension) { // selection reaches right (bottom) edge of widget
        endCoord = (p.descending) ? p.minimum : p.maximum;
    } else {
        endCoord = (p.descending) ? (1 - static_cast<double>(endCoord) / dimension)
                                            * (p.maximum - p.minimum) + p.minimum
                                  : (static_cast<double>(endCoord) / dimension)
                                            * (p.maximum - p.minimum) + p.minimum;
    }
    setRange(startCoord, endCoord);
}

void UniversalAxis::paintEvent(QPaintEvent* e)
{
    QWidget::paintEvent(e);
    const double width = e->rect().width();
    const double height = e->rect().height();
    const double valueWidth = p.maximum - p.minimum;
    const double linePos = 0.5;
    const double relLenghtTickLine = 0.5;
    const double labelAdditionalSpacing = 0.01;
    double linePos_;

    if (p.vertical) { // position of line connecting ticks
        linePos_ = linePos * width;
    } else {
        linePos_ = linePos * height;
    }

    QFlags<Qt::AlignmentFlag> mainAlignment;
    if (p.vertical) { // TODO rework to include text in top of axis
        mainAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    } else {
        mainAlignment = Qt::AlignHCenter | Qt::AlignTop;
    }

    QPainter painter(this);
    QPen pen;
    pen.setCosmetic(true); // width independent from coordinate transformations
    pen.setWidth(1);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    QFont font = painter.font();

    font.setPixelSize(p.fontSize);

    painter.setFont(font);

    if (p.showLine) { // drawing line connecting ticks
        QPolygonF baseline;
        if (p.vertical) {
            baseline << QPointF(linePos * width, 0)
                     << QPointF(linePos * width, height);
        } else {
            baseline << QPointF(0, linePos * height)
                     << QPointF(width, linePos * height);
        }
        painter.drawPolyline(baseline);
    }
    double firstTickPosition = findFirstDivisibleNumber(p.maximum, p.primaryTicksInterval, p.descending);
    double tickPos = firstTickPosition - p.primaryTicksInterval;
    { // drawing line and labels for all ticks except first and last
        int stepNumber = std::round(std::fabs(valueWidth - p.primaryTicksInterval) / p.primaryTicksInterval);
        for (int i = 0; i < stepNumber; i++, tickPos -= p.primaryTicksInterval)
        {
            QPolygonF tick;
            if (p.vertical) {
                tick << QPointF(linePos * width * (1 - relLenghtTickLine), xPos(tickPos))
                     << QPointF(linePos * width * (1 + relLenghtTickLine), xPos(tickPos));
                drawText(painter,
                         QPointF(linePos * width * (1 + relLenghtTickLine + labelAdditionalSpacing), xPos(tickPos)),
                         mainAlignment, QString::number(tickPos, 'f', p.displayPrecision));
            } else {
                tick << QPointF(xPos(tickPos), linePos * height * (1 - relLenghtTickLine))
                << QPointF(xPos(tickPos), linePos * height * (1 + relLenghtTickLine));
                drawText(painter,
                         QPointF(xPos(tickPos), linePos * height * (1 + relLenghtTickLine + labelAdditionalSpacing)),
                         mainAlignment, QString::number(tickPos, 'f', p.displayPrecision));
            }
            painter.drawPolyline(tick);
        }
    }
    double dimension; // dimension used to test if tick is too close to edge
    if (p.vertical) {
        dimension = height;
    } else {
        dimension = width;
    }

    if ((xPos(firstTickPosition) <= 0.0) or (xPos(firstTickPosition) >= dimension)) { // if tick is exactly on edge it is drawn with wider line, otherwise it sometimes disappears
        pen.setWidth(2);
        pen.setWidth(2);
        painter.setPen(pen);
    }

    QPolygonF firstTick;

    if (p.vertical) { // line for first tick
        firstTick << QPointF(linePos * width * (1 - relLenghtTickLine), xPos(firstTickPosition))
                  << QPointF(linePos * width * (1 + relLenghtTickLine), xPos(firstTickPosition));
    } else {
        firstTick << QPointF(xPos(firstTickPosition), linePos * height * (1 - relLenghtTickLine))
                  << QPointF(xPos(firstTickPosition), linePos * height * (1 + relLenghtTickLine));
    }
    painter.drawPolyline(firstTick);

    pen.setWidth(1);
    painter.setPen(pen);

    QFlags<Qt::AlignmentFlag> edgeAlignment = mainAlignment;
    if (p.vertical) { // alignment of first tick label
        QRectF testRect1 = getBoundingRect(painter,
                                           QPointF(linePos_ * (1 + relLenghtTickLine + labelAdditionalSpacing), xPos(firstTickPosition)),
                                           edgeAlignment,
                                           QString::number(firstTickPosition, 'f', p.displayPrecision));
        if (testRect1.y() < 0.0) {
            edgeAlignment = Qt::AlignTop | Qt::AlignLeft;
        } else if (testRect1.y() + testRect1.height() > height) {
            edgeAlignment = Qt::AlignBottom | Qt::AlignLeft;
        }
    } else {
        QRectF testRect1 = getBoundingRect(painter,
                                           QPointF(xPos(firstTickPosition), linePos_ * (1 + relLenghtTickLine + labelAdditionalSpacing)),
                                           edgeAlignment,
                                           QString::number(firstTickPosition, 'f', p.displayPrecision));
        if (testRect1.x() < 0.0) {
            edgeAlignment = Qt::AlignLeft | Qt::AlignTop;
        } else if (testRect1.x() + testRect1.width() > width) {
            edgeAlignment = Qt::AlignRight | Qt::AlignTop;
        }

    }

    if (p.vertical) { // label of first tick
        drawText(painter,
                 QPointF(linePos_ * (1 + relLenghtTickLine + labelAdditionalSpacing), xPos(firstTickPosition)),
                 edgeAlignment,
                 QString::number(firstTickPosition, 'f', p.displayPrecision));
    } else {
        drawText(painter,
                 QPointF(xPos(firstTickPosition), linePos_ * (1 + relLenghtTickLine + labelAdditionalSpacing)),
                 edgeAlignment,
                 QString::number(firstTickPosition, 'f', p.displayPrecision));
    }

    if ((xPos(tickPos) <= 0.0) or (xPos(tickPos) >= dimension)) { // if tick is exactly on edge it is drawn with wider line, otherwise it sometimes disappears
        pen.setWidth(2);
        pen.setWidth(2);
        painter.setPen(pen);
    }

    QPolygonF lastTick; // line for last tick
    if (p.vertical) {
        lastTick << QPointF(linePos_ * (1 - relLenghtTickLine), xPos(tickPos))
                 << QPointF(linePos_ * (1 + relLenghtTickLine), xPos(tickPos));
    } else {
        lastTick << QPointF(xPos(tickPos), linePos_ * (1 - relLenghtTickLine))
                 << QPointF(xPos(tickPos), linePos_ * (1 + relLenghtTickLine));
    }
    painter.drawPolyline(lastTick);

    if (p.vertical) { // alignment of last tick label
        QRectF testRect1 = getBoundingRect(painter,
                                           QPointF(linePos_ * (1 + relLenghtTickLine + labelAdditionalSpacing), xPos(tickPos)),
                                           edgeAlignment,
                                           QString::number(tickPos, 'f', p.displayPrecision));
        if (testRect1.y() < 0.0) {
            edgeAlignment = Qt::AlignTop | Qt::AlignLeft;
        } else if (testRect1.y() + testRect1.height() > height) {
            edgeAlignment = Qt::AlignBottom | Qt::AlignLeft;
        }
    } else {
        QRectF testRect1 = getBoundingRect(painter,
                                           QPointF(xPos(tickPos), linePos_ * (1 + relLenghtTickLine + labelAdditionalSpacing)),
                                           edgeAlignment,
                                           QString::number(tickPos, 'f', p.displayPrecision));
        if (testRect1.x() < 0.0) {
            edgeAlignment = Qt::AlignLeft | Qt::AlignTop;
        } else if (testRect1.x() + testRect1.width() > width) {
            edgeAlignment = Qt::AlignRight | Qt::AlignTop;
        }

    }

    if (p.vertical) { // last tick label
        drawText(painter,
                 QPointF(linePos_ * (1 + relLenghtTickLine + labelAdditionalSpacing), xPos(tickPos)),
                 edgeAlignment,
                 QString::number(tickPos, 'f', p.displayPrecision));
    } else {
        drawText(painter,
                 QPointF(xPos(tickPos), linePos_ * (1 + relLenghtTickLine + labelAdditionalSpacing)),
                 edgeAlignment,
                 QString::number(tickPos, 'f', p.displayPrecision));
    }
    // painter.drawRect(QRectF(0.0, 0.0, width - 1, height - 1));

}



// XAxis::XAxis(XAxisProperties properties, QWidget* parent)
//     : QWidget{parent}
//     , p{properties}
//     , r{(p.decreasingToRight) ? 1 : -1}
// {
//     initialize();
// }

// void XAxis::initialize()
// {
//     if (p.primaryTicksInterval == 0.0) {
//         p.primaryTicksInterval = findInterval(std::fabs(p.left - p.right));
//     }
//     if (p.secondaryTicksInterval == 0.0) {
//         p.secondaryTicksInterval = p.primaryTicksInterval / 2;
//     }
//     if (p.fixedDisplayPrecision) {
//         displayPrecision = *p.fixedDisplayPrecision;
//     } else {
//         displayPrecision = calcDisplayPrecision(p.primaryTicksInterval);
//     }
// }

// void XAxis::setRange(double left, double right)
// {
//     if (p.decreasingToRight and (left < right)) {
//         std::swap(left, right);
//     }
//     if (not p.decreasingToRight and (left > right)) {
//         std::swap(left, right);
//     }

//     p.left = left;
//     p.right = right;
//     p.primaryTicksInterval = 0.0;
//     p.secondaryTicksInterval = 0.0;
//     initialize();
//     update();
// }

// void XAxis::setRangePoints(QPointF left, QPointF right)
// {
//     if (not p.decreasingToRight) return; // TODO version for increasing axis

//     left = mapFromGlobal(left);
//     right = mapFromGlobal(right);

//     double newLeft = (left.x() < 0) ? p.left
//                                     : (1 - static_cast<double>(left.x()) / width())
//                                         * (p.left - p.right) + p.right;

//     double newRight = (right.x() > width()) ? p.right
//                                             : (1 - static_cast<double>(right.x()) / width())
//                                         * (p.left - p.right) + p.right;
//     setRange(newLeft, newRight);
// }

// void XAxis::paintEvent(QPaintEvent* e)
// {
//     QWidget::paintEvent(e);
//     const double width = e->rect().width();
//     const double height = e->rect().height();
//     const double spectralWidth = std::fabs(p.left - p.right);
//     const double lineHeight = p.lineHeight * height;

//     std::function<double(double)> xPos; // converts value in ppm to x coordinate in widget

//     if (p.decreasingToRight) { // would be good to put it in initialize for future!!!
//         xPos = [width, spectralWidth, this](double x) -> double {return (1 - ((x - p.right) / spectralWidth)) * width;};
//     } else {
//         xPos = [width, spectralWidth, this](double x) -> double {return ((x - p.left) / spectralWidth) * width;};
//     }

//     QPainter painter(this);
//     QPen pen;
//     pen.setCosmetic(true); // width independent from coordinate transformations
//     pen.setWidth(1);
//     pen.setCapStyle(Qt::RoundCap);
//     painter.setPen(pen);
//     QFont font = painter.font();
//     double diameterProportion = std::sqrt(std::pow(e->rect().width(), 2) + std::pow(e->rect().height(), 2))
//                               / std::sqrt(std::pow(screen()->availableGeometry().size().width(), 2)
//                               + std::pow(screen()->availableGeometry().size().height(), 2)); // to be put elsewhere not in paintevent?
//     font.setPixelSize(p.fontSize * diameterProportion); // size increases with window size

//     painter.setFont(font);

//     painter.setWindow(0, 0, width, height);

//     if (p.showLine) {
//         QPolygonF baseline;
//         baseline << QPointF(0, lineHeight)
//                  << QPointF(width, lineHeight);
//         painter.drawPolyline(baseline);
//     }

//     double firstTickPosition = findFirstDivisibleNumber(p.left, p.primaryTicksInterval, p.decreasingToRight);
//     double tickPos = firstTickPosition - p.primaryTicksInterval * r;
//     {
//         int stepNumber = std::round(std::fabs(spectralWidth - p.primaryTicksInterval) / p.primaryTicksInterval);
//         for (int i = 0; i < stepNumber; i++, tickPos -= p.primaryTicksInterval * r)
//         {
//             QPolygonF tick;
//             tick << QPointF(xPos(tickPos), lineHeight * (1 - p.relLenghtTickLine))
//                  << QPointF(xPos(tickPos), lineHeight * (1 + p.relLenghtTickLine));
//             painter.drawPolyline(tick);

//             drawText(painter,
//                     QPointF(xPos(tickPos), lineHeight * (1 + p.relLenghtTickLine + p.labelAdditionalSpacing)),
//                     Qt::AlignHCenter | Qt::AlignTop,
//                     QString::number(tickPos, 'f', displayPrecision));
//         }
//     }

//     if ((std::fabs(0 - xPos(firstTickPosition)) < 0.0001 * width)) {
//         pen.setWidth(2);
//         painter.setPen(pen);
//     }

//     QPolygonF firstTick;
//     firstTick << QPointF(xPos(firstTickPosition), lineHeight * (1 - p.relLenghtTickLine))
//               << QPointF(xPos(firstTickPosition), lineHeight * (1 + p.relLenghtTickLine));
//     painter.drawPolyline(firstTick);

//     pen.setWidth(1);
//     painter.setPen(pen);

//     Qt::AlignmentFlag alignment = (std::fabs(0 - xPos(firstTickPosition)) < 0.01 * width) ? Qt::AlignLeft : Qt::AlignHCenter;

//     drawText(painter,
//              QPointF(xPos(firstTickPosition), lineHeight * (1 + p.relLenghtTickLine + p.labelAdditionalSpacing)),
//              alignment | Qt::AlignTop,
//              QString::number(firstTickPosition, 'f', displayPrecision));

//     if (std::fabs(width - xPos(tickPos)) < 0.0001 * width) {
//         pen.setWidth(2);
//         painter.setPen(pen);
//     }

//     QPolygonF lastTick;
//     lastTick << QPointF(xPos(tickPos), lineHeight * (1 - p.relLenghtTickLine))
//              << QPointF(xPos(tickPos), lineHeight * (1 + p.relLenghtTickLine));
//     painter.drawPolyline(lastTick);

//     pen.setWidth(1);
//     painter.setPen(pen);

//     alignment = (std::fabs(width - xPos(tickPos)) < 0.01 * width) ? Qt::AlignRight : Qt::AlignHCenter;

//     drawText(painter,
//              QPointF(xPos(tickPos), lineHeight * (1 + p.relLenghtTickLine + p.labelAdditionalSpacing)),
//              alignment | Qt::AlignTop,
//              QString::number(tickPos, 'f', displayPrecision));

// }
