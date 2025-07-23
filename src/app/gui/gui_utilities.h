#ifndef GUI_UTILITIES_H
#define GUI_UTILITIES_H

#include <QPainter>

// function to draw text in selected point
void drawText(QPainter& painter, qreal x, qreal y, Qt::Alignment flags,
              const QString& text, QRectF* boundingRect = 0);

// function to draw text in selected point
void drawText(QPainter& painter, const QPointF& point, Qt::Alignment flags,
              const QString& text, QRectF* boundingRect = {});

void drawRangeWithMarks(QPainter& painter, const QPointF& left, const QPointF& right, double tickHeight);

// used to obtain bounding rectangle of text which would be drawn by drawText
QRectF getBoundingRect(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
                       const QString & text);

QRectF getBoundingRect(QPainter & painter, const QPointF & point, Qt::Alignment flags,
                       const QString & text);



#endif // GUI_UTILITIES_H
