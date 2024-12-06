#ifndef XAXIS_H
#define XAXIS_H

#include <QObject>
#include <QWidget>

struct XAxisProperties
{
    double left;
    double right;
    double primaryTicksInterval;
    double secondaryTicksInterval;
    double secTickProp;
    double relLenghtTickLine;
    double lineHeight;
};





class XAxis : public QWidget
{
    Q_OBJECT
public:
    XAxis(XAxisProperties properties, QWidget* parent);

    void paintEvent(QPaintEvent* e);

private:
    XAxisProperties p;
};

#endif // XAXIS_H
