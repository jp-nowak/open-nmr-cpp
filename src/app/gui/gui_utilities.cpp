#include"gui_utilities.h"

#include <QPainter>
#include <QDebug>
#include <QRectF>

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

QRectF getBoundingRect(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
                     const QString & text)
{
    constexpr qreal size = 32767.0;
    QPointF corner(x, y - size);
    if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
    else if (flags & Qt::AlignRight) corner.rx() -= size;
    if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
    else if (flags & Qt::AlignTop) corner.ry() += size;
    else flags |= Qt::AlignBottom;
    QRectF rect{corner.x(), corner.y(), size, size};
    return painter.boundingRect(rect, flags, text);
}

QRectF getBoundingRect(QPainter & painter, const QPointF & point, Qt::Alignment flags,
                       const QString & text)
{
    return getBoundingRect(painter, point.x(), point.y(), flags, text);
}

QRectF getTightBoundingRect(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
                     const QString & text)
{
    constexpr qreal size = 32767.0;
    QPointF corner(x, y - size);
    if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
    else if (flags & Qt::AlignRight) corner.rx() -= size;
    if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
    else if (flags & Qt::AlignTop) corner.ry() += size;
    else flags |= Qt::AlignBottom;
    QRectF rect{corner.x(), corner.y(), size, size};
    QFontMetrics fm{painter.font()};
    QRectF initialBounding = painter.boundingRect(rect, flags, text);
    double correction = fm.descent() + fm.underlinePos();

    return QRectF(initialBounding.x(), initialBounding.y() + 0.9 * correction, initialBounding.width(), initialBounding.height() - 1.5 * correction);
}

QRectF getTightBoundingRect(QPainter& painter, const QPointF& point, Qt::Alignment flags,
                       const QString& text)
{
    return getTightBoundingRect(painter, point.x(), point.y(), flags, text);
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

void setWidth(QPainter& painter, int width)
{
    QPen pen = painter.pen();
    pen.setWidth(width);
    painter.setPen(pen);
}

void setFontPixelSize(QPainter& painter, int size)
{
    QFont font = painter.font();
    font.setPixelSize(size);
    painter.setFont(font);
}

int fitFontPixelSize(QPainter& painter, const QPointF& point, Qt::Alignment flags,
                       const QString& text)
{
    QPaintDevice* widget = painter.device();
    assert(widget && "no painting device");
    auto widgetRect = QRectF{0.0, 0.0, static_cast<qreal>(widget->width()), static_cast<qreal>(widget->height())};
    painter.save();
    int size = painter.font().pixelSize();
    for (; 0 <-- size;) {
        setFontPixelSize(painter, size);
        QRectF boundingRect = getTightBoundingRect(painter, point, flags, text);
        if ((boundingRect & widgetRect) == boundingRect) break;
    }
    painter.restore();
    return size;
}




















