#include "xaxis.h"
#include "gui_utilities.h"

#include <cmath>

#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QPointF>
#include <QPaintEvent>
#include <QDebug>
#include <QFont>
#include <QRectF>
#include <QString>

XAxis::XAxis(XAxisProperties properties, QWidget* parent)
: QWidget{parent}
, p{properties}
{
    if (p.primaryTicksInterval == 0.0) {
        p.primaryTicksInterval = [](double x){
            int power10 = std::floor(std::log10(x));
            if (power10 < 0)
            {
                x *= std::pow(10, -power10);
            }
            double interval = std::floor(x / std::pow(10, std::abs(power10))) * std::pow(10, std::abs(power10) - 1);
            if (power10 < 0)
            {
                x /= std::pow(10, -power10);
            }
            return interval;
        }(p.left - p.right);
        qDebug() << p.primaryTicksInterval;
    }
    if (p.secondaryTicksInterval == 0.0) {
        p.secondaryTicksInterval = p.primaryTicksInterval / 2;
        qDebug() << p.secondaryTicksInterval;
    }

    displayPrecision = [](int x){
        double divisor = 1;
        int precision = 1;
        for (; x < divisor; precision++, divisor /=10) {continue;} // empty body
        return precision;
    }(p.primaryTicksInterval);

}

void XAxis::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    QPen pen;

    pen.setCosmetic(true);
    pen.setWidth(1);
    pen.setCapStyle(Qt::RoundCap);

    painter.setPen(pen);

    QFont font = painter.font();

    double multiplier = 100;
    double oryginalWidth = e->rect().width();
    double oryginalHeight = e->rect().height();

    double newHeight = oryginalHeight / oryginalWidth * (p.left - p.right);

    font.setPointSizeF(1 * std::sqrt(std::pow(newHeight, 2) + std::pow(p.left - p.right, 2)));

    painter.setFont(font);

    painter.setWindow((-p.left) * multiplier, 0, (p.left - p.right) * multiplier, newHeight * multiplier);
    painter.scale(-1, 1);

    double firstPTickPosition = [this](double left){
        int power10 = std::floor(std::log10(left));
        int multiplier = 1;
        if (power10 < 0)
        {
            multiplier = std::pow(10, std::abs(power10));
        }
        int leftInteger = std::floor(left * multiplier);
        int primaryTickInterval = p.primaryTicksInterval * multiplier;
        int firstPosition = leftInteger;
        for (; firstPosition % primaryTickInterval; firstPosition--) {continue;} // empty body
        return firstPosition / multiplier;
    }(p.left);

    QPolygonF line;
    line << QPointF(p.left * multiplier, p.lineHeight * newHeight * multiplier)
         << QPointF(p.right * multiplier, p.lineHeight * newHeight * multiplier);

    painter.drawPolyline(line);


    double pTickPos = firstPTickPosition - p.primaryTicksInterval;
    {
        // int stepNumber = std::floor((p.left - p.right - p.primaryTicksInterval) / p.primaryTicksInterval);
        // double j = std::floor(p.left) - p.primaryTicksInterval;
        int stepNumber = std::floor((p.left - p.right - p.primaryTicksInterval) / p.primaryTicksInterval);
        for (int i = 0; i < stepNumber; i++, pTickPos -= p.primaryTicksInterval)
        {
            QPolygonF tick;

            tick << QPointF(pTickPos * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine))
                 << QPointF(pTickPos * multiplier, newHeight * multiplier * (p.lineHeight - p.relLenghtTickLine));
            painter.drawPolyline(tick);
            painter.save();
            painter.scale(-1, 1);
            drawText(painter,
                    QPointF(-pTickPos * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine + p.labelAdditionalSpacing)),
                    Qt::AlignHCenter | Qt::AlignTop,
                    QString::number(pTickPos, 'f', displayPrecision));
            // painter.scale(-1, 1);
            painter.restore();
        }
    }

    {
        int stepNumber = std::floor((p.left - p.right - p.secondaryTicksInterval) / p.secondaryTicksInterval);
        double j = firstPTickPosition - p.secondaryTicksInterval;
        for (int i = 0; i < stepNumber; i++, j-= p.secondaryTicksInterval)
        {
            QPolygonF tick;
            tick << QPointF(j * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine * p.secTickProp))
                 << QPointF(j * multiplier, newHeight * multiplier * (p.lineHeight - p.relLenghtTickLine * p.secTickProp));
            painter.drawPolyline(tick);
        }
    }

    QPolygonF firstTick;
    firstTick << QPointF(firstPTickPosition * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine))
              << QPointF(firstPTickPosition * multiplier, newHeight * multiplier * (p.lineHeight - p.relLenghtTickLine));

    QPolygonF lastTick;
    lastTick << QPointF(pTickPos * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine))
             << QPointF(pTickPos * multiplier, newHeight * multiplier * (p.lineHeight - p.relLenghtTickLine));

    painter.drawPolyline(firstTick);
    painter.drawPolyline(lastTick);

    qDebug() << firstPTickPosition << pTickPos;

    painter.scale(-1, 1);

    Qt::AlignmentFlag alignment = (p.left - firstPTickPosition < 1) ? Qt::AlignLeft : Qt::AlignHCenter;

    drawText(painter, QPointF(-firstPTickPosition * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine + p.labelAdditionalSpacing)),
                    alignment | Qt::AlignTop, QString::number(firstPTickPosition, 'f', displayPrecision));

    alignment = (pTickPos - p.right < 1) ? Qt::AlignRight : Qt::AlignHCenter;

    drawText(painter, QPointF(-pTickPos * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine + p.labelAdditionalSpacing)),
                    alignment | Qt::AlignTop, QString::number(pTickPos, 'f', displayPrecision));

}
