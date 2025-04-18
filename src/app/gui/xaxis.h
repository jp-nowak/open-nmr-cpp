#ifndef XAXIS_H
#define XAXIS_H

#include <QObject>
#include <QWidget>
#include <QPen>

#include <optional>

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
    bool decreasingToRight{true};
    int fontSize{15}; // size of text in ticks labels, 15 is default for spectrum
    std::optional<int> fixedDisplayPrecision{}; // should only be set when setRange wont be used
    bool showLine{true};
};

class XAxis : public QWidget
{
    Q_OBJECT
public:

    friend SpectrumDisplayer;

    XAxis(XAxisProperties properties, QWidget* parent);

    void paintEvent(QPaintEvent*) override;
    void setRange(double left, double right);
    void setRangePoints(QPointF left, QPointF right);



private:
    void initialize();

    XAxisProperties p;
    int displayPrecision;
    int r;

};

#endif // XAXIS_H
