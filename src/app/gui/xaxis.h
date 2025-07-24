#ifndef XAXIS_H
#define XAXIS_H

#include <QObject>
#include <QWidget>
#include <QPen>

#include <optional>

class SpectrumDisplayer;


struct AxisProperties
{
    double minimum; // smallest value of axis
    double maximum; // biggest value of axis
    double primaryTicksInterval{0}; // matters only if dynamic is false
    double secondaryTicksInterval{0}; // matters only if dynamic is false
    double primaryToSecondaryTickRatio{2}; // matters only if dynamic is true
    unsigned short fontSize{15};
    bool showLine{true}; // if true line connecting ticks is displayed
    unsigned short displayPrecision{0};
    bool vertical; // if true axis is vertical if false horizontal
    bool descending; // if true vertical axis has biggest value on the top, horizontal on left side
    bool dynamic; // if true setRange methods can be used to change values, if false displayPrecision can be used to specify fixed display precision
    double relTickLine{0.25}; // width of tick relative to shorter dimension of widget
    double linePos{0.5}; // position of line connecting ticks as fraction of longer dimension
    double labelAdditionalSpacing{0.01};
};

class UniversalAxis : public QWidget
{
    Q_OBJECT
public:
    UniversalAxis(AxisProperties properties, QWidget* parent);
    void paintEvent(QPaintEvent*) override;
    void setRange(double minimum, double maximum);
    void setRangePoints(QPointF left, QPointF right);

    /*!
     * \brief getRange returns current range of axis
     * \return pair of {p.maximum, p.minimum}
     */
    std::pair<double, double> getRange();

    /*!
     * \brief xPos returns coordinate of point corresponding to value on axis
     * if axis is vertical it is y coordinate else x coordinate
     * \param
     * \return
     */
    double xPos(double x);
private:
    void initialize();

    AxisProperties p;
};


// struct XAxisProperties
// {
//     double left;
//     double right;
//     double primaryTicksInterval;
//     double secondaryTicksInterval;
//     double secTickProp;
//     double relLenghtTickLine;
//     double lineHeight;
//     double labelAdditionalSpacing;
//     bool decreasingToRight{true};
//     int fontSize{15}; // size of text in ticks labels, 15 is default for spectrum
//     std::optional<int> fixedDisplayPrecision{}; // should only be set when setRange wont be used
//     bool showLine{true};
// };

// class XAxis : public QWidget
// {
//     Q_OBJECT
// public:

//     friend SpectrumDisplayer;

//     XAxis(XAxisProperties properties, QWidget* parent);

//     void paintEvent(QPaintEvent*) override;
//     void setRange(double left, double right);
//     void setRangePoints(QPointF left, QPointF right);



// private:
//     void initialize();

//     XAxisProperties p;
//     int displayPrecision;
//     int r;

// };




#endif // XAXIS_H
