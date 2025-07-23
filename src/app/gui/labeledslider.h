#ifndef LABELEDSLIDER_H
#define LABELEDSLIDER_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSignalBlocker>

class UniversalAxis;
class QHBoxLayout;
class QSlider;
class QDoubleSpinBox;
class QLabel;

class LabeledSlider : public QWidget
{
    Q_OBJECT

public:

    LabeledSlider(double minimum, double maximum,
                  double interval, unsigned int precision,
                  double initial,
                  QWidget* parent);

    void setValue(double value);

    static std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> createWithBoxAndLabel(QString label, double minimum, double maximum,
                                                       double interval, unsigned int precision, double initial, QWidget* parent);

    static void changeValues(std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> i, double value);

private:
    double multiplier;
    QVBoxLayout* layout;
    UniversalAxis* labels;
    QSlider* slider;

signals:
    void sliderMoved(double value);

private slots:
    void sliderMovedSlot(int value);


};




// class SliderWithEditField : public QWidget
// {
//     Q_OBJECT

// public:

//     SliderWithEditField(double minimum, double maximum,
//                   double interval, unsigned int precision,
//                   double initial, QString label,
//                   QWidget* parent);

// private:

//     QHBoxLayout* layout;
//     QLabel* label;
//     LabeledSlider* slider;
//     QDoubleSpinBox* box;

// signals:
//     void valueChanged(double value);

// private slots:
//     void valueChangedSlot(double value);

// };









#endif // LABELEDSLIDER_H
