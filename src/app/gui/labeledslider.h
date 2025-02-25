#ifndef LABELEDSLIDER_H
#define LABELEDSLIDER_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSignalBlocker>

class XAxis;
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
                                                       double interval, unsigned int precision,
                                                       double initial,
                                                       QWidget* parent)
    {
        QWidget* mainWidget = new QWidget(parent);
        QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
        QHBoxLayout* subLayout = new QHBoxLayout(nullptr);
        QLabel* qlabel = new QLabel(label, mainWidget);
        QDoubleSpinBox* box = new QDoubleSpinBox(mainWidget);
        box->setMinimum(minimum);
        box->setMaximum(maximum);
        box->setDecimals(precision);
        box->setValue(initial);
        LabeledSlider* slider = new LabeledSlider(minimum, maximum, interval, precision, initial, mainWidget);
        connect(slider, &LabeledSlider::sliderMoved, [box](double i)
                {
                    QSignalBlocker blocker(box);
                    box->setValue(i);
                });
        connect(box, &QDoubleSpinBox::valueChanged, [slider](double i)
                {
                    QSignalBlocker blocker(slider);
                    slider->setValue(i);
                });
        subLayout->addWidget(qlabel);
        subLayout->addWidget(box);
        mainLayout->addLayout(subLayout);
        mainLayout->addWidget(slider);
        return {mainWidget, slider, box};
    }

    static void changeValues(std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> i, double value)
    {
        QSignalBlocker blocker1(std::get<LabeledSlider*>(i));
        QSignalBlocker blocker2(std::get<QDoubleSpinBox*>(i));
        std::get<LabeledSlider*>(i)->setValue(value);
        std::get<QDoubleSpinBox*>(i)->setValue(value);
    }


private:
    double multiplier;
    QVBoxLayout* layout;
    XAxis* labels;
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
