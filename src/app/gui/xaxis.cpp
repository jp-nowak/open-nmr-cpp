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
        double multiplier = 1;
        while (span <= 0.01) span *= 10, multiplier /= 10;
        while (span >= 1000) span /= 10, multiplier *= 10;

        auto inRange = [span](double low, double high){return !(span < low) && (span < high);};

        double result = 1;

        if (inRange(5, 1000)) {

            if (inRange(5, 10)) result = 1;
            if (inRange(10, 20)) result = 2;
            if (inRange(20, 30)) result = 3;
            if (inRange(30, 40)) result = 4;
            if (inRange(40, 50)) result = 5;
            if (inRange(50, 60)) result = 6;
            if (inRange(60, 70)) result = 7;
            if (inRange(70, 80)) result = 8;
            if (inRange(80, 90)) result = 9;
            if (inRange(90, 150)) result = 10;
            if (inRange(150, 250)) result = 20;
            if (inRange(250, 350)) result = 30;
            if (inRange(350, 450)) result = 40;
            if (inRange(450, 550)) result = 50;
            if (inRange(550, 650)) result = 60;
            if (inRange(650, 1000)) result = 100;
        }

        if (inRange(1, 5)) {
            if (inRange(1, 2)) result = 0.2;
            if (inRange(2, 3)) result = 0.3;
            if (inRange(3, 4)) result = 0.4;
            if (inRange(4, 5)) result = 0.5;
        }

        if (inRange(0.01, 1)) {
            if (inRange(0.01, 0.1)) result = 0.01;
            if (inRange(0.10, 0.20)) result = 0.02;
            if (inRange(0.20, 0.30)) result = 0.03;
            if (inRange(0.30, 0.40)) result = 0.04;
            if (inRange(0.40, 0.50)) result = 0.05;
            if (inRange(0.50, 0.60)) result = 0.06;
            if (inRange(0.60, 0.70)) result = 0.07;
            if (inRange(0.70, 0.80)) result = 0.08;
            if (inRange(0.80, 0.90)) result = 0.09;
            if (inRange(0.90, 1.00)) result = 0.10;
        }

        return result * multiplier;

        // return [](double x){
        //     int power10 = std::floor(std::log10(x));
        //     if (power10 < 0)
        //     {
        //         x *= std::pow(10, -power10);
        //     }
        //     int interval = std::ceil(x);
        //     double result = interval / std::pow(10, std::abs(std::ceil(std::log10(interval))));

        //     int power10Result = std::floor(std::log10(result));
        //     if (power10Result < 0)
        //     {
        //         result *= std::pow(10, -power10Result);
        //     }
        //     result = std::round(result);
        //     if (power10Result < 0)
        //     {
        //         result /= std::pow(10, -power10Result);
        //     }

        //     return (power10 < 0) ? result / std::pow(10, -power10) : result;
        // }(span);
    }

    int calcDisplayPrecision(double x)
    {
        double divisor = 1;
        int precision = 1;
        if (x > 10) return 0;
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

    class P : public QPointF
    {
        public:
        using QPointF::QPointF;
        QPointF transposeIf(bool p)
        {
            if (p) return this->transposed(); else return *this;
        }
    };

}


UniversalAxis::UniversalAxis(AxisProperties properties, QWidget* parent)
    : p{properties}
{

    assert(parent && "nullptr to parent");
    assert(p.minimum != p.maximum);
    initialize();
}

void UniversalAxis::initialize()
{

    assert(p.minimum < p.maximum);

    if (not p.dynamic and not positions.empty()) return;

    if (p.dynamic) {
        p.primaryTicksInterval = findInterval(p.maximum - p.minimum);
        p.secondaryTicksInterval = p.primaryTicksInterval / p.primaryToSecondaryTickRatio;
        p.displayPrecision = calcDisplayPrecision(p.primaryTicksInterval);
    }

    positions.clear();
    labels.clear();

    for (double value = findFirstDivisibleNumber(p.maximum, p.primaryTicksInterval, p.descending);
        value >= p.minimum; value -= p.primaryTicksInterval) {
        positions.append(value);
        labels.append(QString::number(value, 'f', p.displayPrecision));
        }

}

double UniversalAxis::xPos(double x)
{
    const double valueWidth = p.maximum - p.minimum;
    const double longestDimension = (p.vertical) ? height() : width();
    if (p.descending) {
        return (1 - ((x - p.minimum) / valueWidth)) * longestDimension;
    } else {
        return ((x - p.minimum) / valueWidth) * longestDimension;
    }
}

QPolygonF UniversalAxis::getPointsAsGlobals()
{
    QPolygonF points;
    const double shorterDimension = (p.vertical) ? width() : height();
    const double linePos_ = p.linePos * shorterDimension;
    for (auto x : std::as_const(positions)) {
        points << mapToGlobal(P(x, linePos_).transposeIf(p.vertical));
    }
    return points;
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

std::pair<double, double> UniversalAxis::getRange()
{
    return {p.maximum, p.minimum};
}

void UniversalAxis::paintEvent(QPaintEvent* e)
{
    assert(not positions.empty());
    assert(not labels.empty());

    QWidget::paintEvent(e);
    const double width = e->rect().width();
    const double height = e->rect().height();
    const double linePos = p.linePos;
    const double relLenghtTickLine = p.relTickLine / linePos;
    const double labelAdditionalSpacing = p.labelAdditionalSpacing / linePos;
    const double shorterDimension = (p.vertical) ? width : height;
    const double longerDimension = (p.vertical) ? height : width;
    const double linePos_ = p.linePos * shorterDimension;

    QFlags<Qt::AlignmentFlag> mainAlignment, secondaryAlignment;

    if (p.labelAdditionalSpacing > 0) {
        secondaryAlignment = (p.vertical) ? Qt::AlignLeft : Qt::AlignTop;
    } else {
        secondaryAlignment = (p.vertical) ? Qt::AlignRight : Qt::AlignBottom;
    }

    mainAlignment = (p.vertical) ? (Qt::AlignVCenter | secondaryAlignment) : (Qt::AlignHCenter | secondaryAlignment);

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
        painter.drawLine(P(0, linePos_).transposeIf(p.vertical), P(longerDimension, linePos_).transposeIf(p.vertical));
    }

    const double labelPosition = (relLenghtTickLine + labelAdditionalSpacing) * ((p.labelAdditionalSpacing > 0) ? 1 : -1);

    // shrinks font size if needed for label to fit in widget area
    setFontPixelSize(painter, fitFontPixelSize(painter, P(xPos(positions[1]), linePos_ * (1 + labelPosition)).transposeIf(p.vertical), mainAlignment, labels[1]));

    for (int i = 1; i < positions.size() - 1; i++) // drawing ticks and labels except first and last
    {
        // tick line
        painter.drawLine(P(xPos(positions[i]), linePos_ * (1 - relLenghtTickLine) ).transposeIf(p.vertical),
                         P(xPos(positions[i]), linePos_ * (1 + relLenghtTickLine) ).transposeIf(p.vertical));
        // label
        drawText(painter, P(xPos(positions[i]), linePos_ * (1 + labelPosition)).transposeIf(p.vertical), mainAlignment, labels[i]);
    }

    painter.save();

    // drawing first tick line, if line is on the edge it is doubled (otherwise it disappears)
    if ((xPos(positions[0]) <= 0.0) or (xPos(positions[0]) >= longerDimension)) {
        setWidth(painter, 2);
    }
    painter.drawLine(P(xPos(positions[0]), linePos_ * (1 - relLenghtTickLine) ).transposeIf(p.vertical),
                     P(xPos(positions[0]), linePos_ * (1 + relLenghtTickLine) ).transposeIf(p.vertical));
    painter.restore();

    painter.save();

    // drawing last tick line, if line is on the edge it is doubled (otherwise it disappears)
    if ((xPos(positions.last()) <= 0.0) or (xPos(positions.last()) >= longerDimension)) {
        setWidth(painter, 2);
    }
    painter.drawLine(P(xPos(positions.last()), linePos_ * (1 - relLenghtTickLine) ).transposeIf(p.vertical),
                     P(xPos(positions.last()), linePos_ * (1 + relLenghtTickLine) ).transposeIf(p.vertical));
    painter.restore();

    QFlags<Qt::AlignmentFlag> edgeAlignment = mainAlignment;

    // checking if label fits, if not alignment is shifted so it does fit
    QRectF testRect = getBoundingRect(painter,
                                      P(xPos(positions[0]), linePos_ * (1 + labelPosition) ).transposeIf(p.vertical),
                                      edgeAlignment, labels[0]);

    double testRectDimension = (p.vertical) ? testRect.y() : testRect.x();

    if (testRectDimension < 0.0) {
            edgeAlignment = (p.vertical) ? (Qt::AlignTop | secondaryAlignment) : (Qt::AlignLeft | secondaryAlignment);
    } else if (testRectDimension += (p.vertical) ? testRect.height() : testRect.width();
               testRectDimension > longerDimension) {
            edgeAlignment = (p.vertical) ? (Qt::AlignBottom | secondaryAlignment) : (Qt::AlignRight | secondaryAlignment);
    }
    drawText(painter, P(xPos(positions[0]), linePos_ * (1 + labelPosition) ).transposeIf(p.vertical),
            edgeAlignment, labels[0]);

    edgeAlignment = mainAlignment;

    testRect = getBoundingRect(painter,
                                      P(xPos(positions.last()), linePos_ * (1 + labelPosition) ).transposeIf(p.vertical),
                                      edgeAlignment, labels.last());

    testRectDimension = (p.vertical) ? testRect.y() : testRect.x();

    if (testRectDimension < 0.0) {
            edgeAlignment = (p.vertical) ? (Qt::AlignTop | secondaryAlignment) : (Qt::AlignLeft | secondaryAlignment);
    } else if (testRectDimension += (p.vertical) ? testRect.height() : testRect.width();
               testRectDimension > longerDimension) {
            edgeAlignment = (p.vertical) ? (Qt::AlignBottom | secondaryAlignment) : (Qt::AlignRight | secondaryAlignment);
    }
    drawText(painter, P(xPos(positions.last()), linePos_ * (1 + labelPosition) ).transposeIf(p.vertical),
            edgeAlignment, labels.last());

}




