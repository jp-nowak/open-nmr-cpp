#ifndef XAXIS_H
#define XAXIS_H

#include <QObject>
#include <QWidget>
#include <QPen>

class SpectrumDisplayer;

struct XAxisProperties
{
    double left;
    double right;
    double primaryTicksInterval;
    double secondaryTicksInterval;
    double secTickProp;
    double relLenghtTickLine;
    double lineHeight;
    double labelAdditionalSpacing;
};

class XAxis : public QWidget
{
    Q_OBJECT
public:

    friend SpectrumDisplayer;

    XAxis(XAxisProperties properties, QWidget* parent);

    void paintEvent(QPaintEvent*);
    void setRange(double left, double right);
    void setRangePoints(QPointF left, QPointF right);



private:
    void initialize();

    XAxisProperties p;
    int displayPrecision;


};

#endif // XAXIS_H
