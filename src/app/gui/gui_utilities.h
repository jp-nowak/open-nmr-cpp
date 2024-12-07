#ifndef GUI_UTILITIES_H
#define GUI_UTILITIES_H

#include <QPainter>

void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect = 0);

void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect = {});


#endif // GUI_UTILITIES_H
