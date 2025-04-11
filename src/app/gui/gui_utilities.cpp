#include"gui_utilities.h"

#include <QPainter>
#include <QDebug>

// https://github.com/KubaO/stackoverflown/tree/master/questions/alignments-24831484
void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect)
{
   constexpr qreal size = 32767.0;
   QPointF corner(x, y - size);
   if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
   else if (flags & Qt::AlignRight) corner.rx() -= size;
   if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
   else if (flags & Qt::AlignTop) corner.ry() += size;
   else flags |= Qt::AlignBottom;
   QRectF rect{corner.x(), corner.y(), size, size};
   painter.drawText(rect, flags, text, boundingRect);
}

void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect)
{
   drawText(painter, point.x(), point.y(), flags, text, boundingRect);
}

void drawRangeWithMarks(QPainter& painter, const QPointF& left, const QPointF& right, double tickHeight)
{
    {
        QPolygonF line;
        line << left << right;
        painter.drawPolyline(line);
    }

    {
        QPolygonF line;
        line << QPointF(left.x(), left.y() - tickHeight)
             << QPointF(left.x(), left.y() + tickHeight);
        painter.drawPolyline(line);
    }

    {
        QPolygonF line;
        line << QPointF(right.x(), right.y() - tickHeight)
             << QPointF(right.x(), right.y() + tickHeight);
        painter.drawPolyline(line);
    }
}
