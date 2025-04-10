#ifndef INTEGRALSDISPLAYER_H
#define INTEGRALSDISPLAYER_H

#include <QObject>
#include <QWidget>
#include <QPen>
#include <QColor>
#include <QFont>

class Spectrum;
class QDoubleSpinBox;
class IntegralRecord;

struct IntegralsDisplayerProperties // not yet used
{
    double baselineHeight;
    int displayPrecision;
    double tickHeight;
    QPen integralMarksPen;
    QPen integralTextPen;
    QFont integralTextFont;
};


class IntegralsDisplayer : public QWidget
{
    Q_OBJECT
public:
    explicit IntegralsDisplayer(const Spectrum* integrals, QWidget *parent);

    void setRange(size_t begin, size_t end);
    void zoom(QPointF startPos, QPointF endPos);
    void resetZoom();


    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;




private:
    const Spectrum* experiment;

    QDoubleSpinBox* integralEditField;
    IntegralRecord* editedIntegral;

    size_t startPoint_;
    size_t endPoint_;
    size_t currentSpectrumSize;
    QPen pen;


    void recalculateDisplayRange();


};

#endif // INTEGRALSDISPLAYER_H
