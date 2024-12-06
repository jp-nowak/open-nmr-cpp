#include "xaxis.h"

#include <cmath>

#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QPointF>
#include <QPaintEvent>
#include <QDebug>

XAxis::XAxis(XAxisProperties properties, QWidget* parent)
: QWidget{parent}
, p{properties}
{
}

void XAxis::paintEvent(QPaintEvent* e)
{

    QPainter painter(this);
    QPen pen;

    pen.setCosmetic(true);
    pen.setWidth(1);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);



    painter.setWindow(-p.left, 0, p.left - p.right, 100);
    painter.scale(-1, 1);

    QPolygonF line;
    line << QPointF(p.left, p.lineHeight)
         << QPointF(p.right, p.lineHeight);

    painter.drawPolyline(line);

    {
        int stepNumber = std::floor((p.left - p.right - p.primaryTicksInterval) / p.primaryTicksInterval);
        double j = std::floor(p.left) - p.primaryTicksInterval;
        for (int i = 0; i < stepNumber; i++, j-= p.primaryTicksInterval)
        {
            QPolygonF tick;
            tick << QPointF(j, p.lineHeight + 100 * p.relLenghtTickLine)
                 << QPointF(j, p.lineHeight - 100 * p.relLenghtTickLine);
            qDebug() << tick;
            painter.drawPolyline(tick);
        }
    }

    {
        int stepNumber = std::floor((p.left - p.right - p.secondaryTicksInterval) / p.secondaryTicksInterval);
        double j = std::floor(p.left) - p.secondaryTicksInterval;
        for (int i = 0; i < stepNumber; i++, j-= p.secondaryTicksInterval)
        {
            QPolygonF tick;
            tick << QPointF(j, p.lineHeight + 100 * p.relLenghtTickLine * p.secTickProp)
                 << QPointF(j, p.lineHeight - 100 * p.relLenghtTickLine * p.secTickProp);

            painter.drawPolyline(tick);
        }
    }

    QPolygonF firstTick;
    firstTick << QPointF(std::floor(p.left), p.lineHeight + 100 * p.relLenghtTickLine)
              << QPointF(std::floor(p.left), p.lineHeight - 100 * p.relLenghtTickLine);

    QPolygonF lastTick;
    lastTick << QPointF(std::floor(p.right) + 1, p.lineHeight + 100 * p.relLenghtTickLine)
              << QPointF(std::floor(p.right) + 1, p.lineHeight - 100 * p.relLenghtTickLine);


    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawPolyline(firstTick);
    painter.drawPolyline(lastTick);
    pen.setWidth(1);
}
