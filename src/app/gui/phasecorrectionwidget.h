#ifndef PHASECORRECTIONWIDGET_H
#define PHASECORRECTIONWIDGET_H

#include <QObject>
#include <QWidget>

class QDoubleSpinBox;

class Spectrum;
class LabeledSlider;

class PhaseCorrectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PhaseCorrectionWidget(Spectrum* experiment, QWidget *parent = nullptr);

    void changeActiveExperiment(Spectrum* experiment);

signals:
    void signalToRefreshDisplayedExperiment();

private:
    Spectrum* experiment;
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> ph0;
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> ph1;
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> pivot;

private slots:
    void ph0Slot(double phase);
    void ph1Slot(double phase);
};

#endif // PHASECORRECTIONWIDGET_H
