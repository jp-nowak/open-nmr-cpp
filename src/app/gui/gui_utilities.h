#ifndef GUI_UTILITIES_H
#define GUI_UTILITIES_H

#include<QPainter>
//! function to draw text in selected point
void drawText(QPainter& painter, qreal x, qreal y, Qt::Alignment flags,
              const QString& text, QRectF* boundingRect = nullptr);

//! function to draw text in selected point
void drawText(QPainter& painter, const QPointF& point, Qt::Alignment flags,
              const QString& text, QRectF* boundingRect = nullptr);

//! draws text rotated by 90 degree clockwise, with alignments as if not rotated
void drawRotatedText90R(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
                        const QString & text, QRectF * boundingRect = nullptr);

//! draws |------| starting in left and ending in rigth, tick height is absolute value
void drawRangeWithMarks(QPainter& painter, const QPointF& left, const QPointF& right, double tickHeight);

//! used to obtain bounding rectangle of text
QRectF getBoundingRect(QPainter& painter, qreal x, qreal y, Qt::Alignment flags,
                       const QString& text);

//! used to obtain bounding rectangle of text
QRectF getBoundingRect(QPainter& painter, const QPointF& point, Qt::Alignment flags,
                       const QString& text);

//! used to obtain minimal rectangle in which text fits, exluding accents
QRectF getTightBoundingRect(QPainter& painter, qreal x, qreal y, Qt::Alignment flags,
                       const QString& text);

//! used to obtain minimal rectangle in which text fits, exluding accents
QRectF getTightBoundingRect(QPainter& painter, const QPointF& point, Qt::Alignment flags,
                       const QString& text);

//! sets width of pen used by painter
void setWidth(QPainter& painter, int width);

//! sets pixel size of font used by painter
void setFontPixelSize(QPainter& painter, int size);

//! returns biggest possible font pixel size such that text written in the point fits in borders of widget
int fitFontPixelSize(QPainter& painter, const QPointF& point, Qt::Alignment flags,
                       const QString& text);



#endif // GUI_UTILITIES_H
