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

    font.setPointSize(10);

    painter.setFont(font);

    double multiplier = 100;
    double oryginalWidth = e->rect().width();
    double oryginalHeight = e->rect().height();

    double newHeight = oryginalHeight / oryginalWidth * (p.left - p.right);

    painter.setWindow((-p.left) * multiplier, 0, (p.left - p.right) * multiplier, newHeight * multiplier);
    painter.scale(-1, 1);




    QPolygonF line;
    line << QPointF(p.left * multiplier, p.lineHeight * newHeight * multiplier)
         << QPointF(p.right * multiplier, p.lineHeight * newHeight * multiplier);

    painter.drawPolyline(line);

    {
        int stepNumber = std::floor((p.left - p.right - p.primaryTicksInterval) / p.primaryTicksInterval);
        double j = std::floor(p.left) - p.primaryTicksInterval;
        for (int i = 0; i < stepNumber; i++, j-= p.primaryTicksInterval)
        {
            QPolygonF tick;

            tick << QPointF(j * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine))
                 << QPointF(j * multiplier, newHeight * multiplier * (p.lineHeight - p.relLenghtTickLine));
            painter.drawPolyline(tick);

            painter.scale(-1, 1);
            drawText(painter,
                    QPointF(-j * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine + p.labelAdditionalSpacing)),
                    Qt::AlignHCenter,
                    QString::number(j, 'f', 1));
            painter.scale(-1, 1);
        }
    }

    {
        int stepNumber = std::floor((p.left - p.right - p.secondaryTicksInterval) / p.secondaryTicksInterval);
        double j = std::floor(p.left) - p.secondaryTicksInterval;
        for (int i = 0; i < stepNumber; i++, j-= p.secondaryTicksInterval)
        {
            QPolygonF tick;
            tick << QPointF(j * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine * p.secTickProp))
                 << QPointF(j * multiplier, newHeight * multiplier * (p.lineHeight - p.relLenghtTickLine * p.secTickProp));
            painter.drawPolyline(tick);
        }
    }

    QPolygonF firstTick;
    firstTick << QPointF(std::floor(p.left) * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine))
              << QPointF(std::floor(p.left) * multiplier, newHeight * multiplier * (p.lineHeight - p.relLenghtTickLine));

    QPolygonF lastTick;
    lastTick << QPointF((std::floor(p.right) + 1) * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine))
             << QPointF((std::floor(p.right) + 1) * multiplier, newHeight * multiplier * (p.lineHeight - p.relLenghtTickLine));

    painter.drawPolyline(firstTick);
    painter.drawPolyline(lastTick);

    painter.scale(-1, 1);
    drawText(painter, QPointF(-std::floor(p.left) * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine + p.labelAdditionalSpacing)),
                    Qt::AlignLeft, QString::number(std::floor(p.left), 'f', 1));

    drawText(painter, QPointF(-(std::floor(p.right) + 1) * multiplier, newHeight * multiplier * (p.lineHeight + p.relLenghtTickLine + p.labelAdditionalSpacing)),
                    Qt::AlignRight, QString::number(std::floor(p.right) + 1, 'f', 1));

}
