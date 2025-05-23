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
#include <functional>
// to be rewritten in more sensible way

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


XAxis::XAxis(XAxisProperties properties, QWidget* parent)
    : QWidget{parent}
    , p{properties}
    , r{(p.decreasingToRight) ? 1 : -1}
{
    initialize();
}

void XAxis::initialize()
{
    if (p.primaryTicksInterval == 0.0) {
        p.primaryTicksInterval = findInterval(std::fabs(p.left - p.right));
    }
    if (p.secondaryTicksInterval == 0.0) {
        p.secondaryTicksInterval = p.primaryTicksInterval / 2;
    }
    if (p.fixedDisplayPrecision) {
        displayPrecision = *p.fixedDisplayPrecision;
    } else {
        displayPrecision = calcDisplayPrecision(p.primaryTicksInterval);
    }
}

void XAxis::setRange(double left, double right)
{
    if (p.decreasingToRight and (left < right)) {
        std::swap(left, right);
    }
    if (not p.decreasingToRight and (left > right)) {
        std::swap(left, right);
    }

    p.left = left;
    p.right = right;
    p.primaryTicksInterval = 0.0;
    p.secondaryTicksInterval = 0.0;
    initialize();
    update();
}

void XAxis::setRangePoints(QPointF left, QPointF right)
{
    if (not p.decreasingToRight) return; // TODO version for increasing axis

    left = mapFromGlobal(left);
    right = mapFromGlobal(right);

    double newLeft = (left.x() < 0) ? p.left
                                    : (1 - static_cast<double>(left.x()) / width())
                                        * (p.left - p.right) + p.right;

    double newRight = (right.x() > width()) ? p.right
                                            : (1 - static_cast<double>(right.x()) / width())
                                        * (p.left - p.right) + p.right;
    setRange(newLeft, newRight);
}




void XAxis::paintEvent(QPaintEvent* e)
{
    QWidget::paintEvent(e);
    const double width = e->rect().width();
    const double height = e->rect().height();
    const double spectralWidth = std::fabs(p.left - p.right);
    const double lineHeight = p.lineHeight * height;

    std::function<double(double)> xPos; // converts value in ppm to x coordinate in widget

    if (p.decreasingToRight) { // would be good to put it in initialize for future!!!
        xPos = [width, spectralWidth, this](double x) -> double {return (1 - ((x - p.right) / spectralWidth)) * width;};
    } else {
        xPos = [width, spectralWidth, this](double x) -> double {return ((x - p.left) / spectralWidth) * width;};
    }

    QPainter painter(this);
    QPen pen;
    pen.setCosmetic(true); // width independent from coordinate transformations
    pen.setWidth(1);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    QFont font = painter.font();
    double diameterProportion = std::sqrt(std::pow(e->rect().width(), 2) + std::pow(e->rect().height(), 2))
                              / std::sqrt(std::pow(screen()->availableGeometry().size().width(), 2)
                              + std::pow(screen()->availableGeometry().size().height(), 2)); // to be put elsewhere not in paintevent?
    font.setPixelSize(p.fontSize * diameterProportion); // size increases with window size

    painter.setFont(font);

    painter.setWindow(0, 0, width, height);

    if (p.showLine) {
        QPolygonF baseline;
        baseline << QPointF(0, lineHeight)
                 << QPointF(width, lineHeight);
        painter.drawPolyline(baseline);
    }

    double firstTickPosition = findFirstDivisibleNumber(p.left, p.primaryTicksInterval, p.decreasingToRight);
    double tickPos = firstTickPosition - p.primaryTicksInterval * r;
    {
        int stepNumber = std::round(std::fabs(spectralWidth - p.primaryTicksInterval) / p.primaryTicksInterval);
        for (int i = 0; i < stepNumber; i++, tickPos -= p.primaryTicksInterval * r)
        {
            QPolygonF tick;
            tick << QPointF(xPos(tickPos), lineHeight * (1 - p.relLenghtTickLine))
                 << QPointF(xPos(tickPos), lineHeight * (1 + p.relLenghtTickLine));
            painter.drawPolyline(tick);

            drawText(painter,
                    QPointF(xPos(tickPos), lineHeight * (1 + p.relLenghtTickLine + p.labelAdditionalSpacing)),
                    Qt::AlignHCenter | Qt::AlignTop,
                    QString::number(tickPos, 'f', displayPrecision));
        }
    }

    if ((std::fabs(0 - xPos(firstTickPosition)) < 0.0001 * width)) {
        pen.setWidth(2);
        painter.setPen(pen);
    }

    QPolygonF firstTick;
    firstTick << QPointF(xPos(firstTickPosition), lineHeight * (1 - p.relLenghtTickLine))
              << QPointF(xPos(firstTickPosition), lineHeight * (1 + p.relLenghtTickLine));
    painter.drawPolyline(firstTick);

    pen.setWidth(1);
    painter.setPen(pen);

    Qt::AlignmentFlag alignment = (std::fabs(0 - xPos(firstTickPosition)) < 0.01 * width) ? Qt::AlignLeft : Qt::AlignHCenter;

    drawText(painter,
             QPointF(xPos(firstTickPosition), lineHeight * (1 + p.relLenghtTickLine + p.labelAdditionalSpacing)),
             alignment | Qt::AlignTop,
             QString::number(firstTickPosition, 'f', displayPrecision));

    if (std::fabs(width - xPos(tickPos)) < 0.0001 * width) {
        pen.setWidth(2);
        painter.setPen(pen);
    }

    QPolygonF lastTick;
    lastTick << QPointF(xPos(tickPos), lineHeight * (1 - p.relLenghtTickLine))
             << QPointF(xPos(tickPos), lineHeight * (1 + p.relLenghtTickLine));
    painter.drawPolyline(lastTick);

    pen.setWidth(1);
    painter.setPen(pen);

    alignment = (std::fabs(width - xPos(tickPos)) < 0.01 * width) ? Qt::AlignRight : Qt::AlignHCenter;

    drawText(painter,
             QPointF(xPos(tickPos), lineHeight * (1 + p.relLenghtTickLine + p.labelAdditionalSpacing)),
             alignment | Qt::AlignTop,
             QString::number(tickPos, 'f', displayPrecision));

}
