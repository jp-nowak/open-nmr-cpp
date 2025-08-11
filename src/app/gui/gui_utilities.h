#ifndef GUI_UTILITIES_H
#define GUI_UTILITIES_H

#include<QPainter>
// function to draw text in selected point
void drawText(QPainter& painter, qreal x, qreal y, Qt::Alignment flags,
              const QString& text, QRectF* boundingRect = 0);

// function to draw text in selected point
void drawText(QPainter& painter, const QPointF& point, Qt::Alignment flags,
              const QString& text, QRectF* boundingRect = {});

void drawRangeWithMarks(QPainter& painter, const QPointF& left, const QPointF& right, double tickHeight);

/*!
 * \brief getBoundingRect used to obtain bounding rectangle of text
 */
QRectF getBoundingRect(QPainter& painter, qreal x, qreal y, Qt::Alignment flags,
                       const QString& text);

/*!
 * \brief getBoundingRect used to obtain bounding rectangle of text
 */
QRectF getBoundingRect(QPainter& painter, const QPointF& point, Qt::Alignment flags,
                       const QString& text);

QRectF getTightBoundingRect(QPainter& painter, qreal x, qreal y, Qt::Alignment flags,
                       const QString& text);

QRectF getTightBoundingRect(QPainter& painter, const QPointF& point, Qt::Alignment flags,
                       const QString& text);

void setWidth(QPainter& painter, int width);

void setFontPixelSize(QPainter& painter, int size);

int fitFontPixelSize(QPainter& painter, const QPointF& point, Qt::Alignment flags,
                       const QString& text);



#endif // GUI_UTILITIES_H
