#ifndef WIDGETS_FOR_PROCESSING_H
#define WIDGETS_FOR_PROCESSING_H

#include <QObject>
#include <QWidget>

class Spectrum;
class MainWindow;

class ProcessingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessingWidget(Spectrum* experiment, QWidget *parent = nullptr);

public slots:
    virtual void changeActiveExperiment(Spectrum* experiment) = 0;

signals:
    void signalToRefreshDisplayedExperiment();

protected:
    Spectrum* experiment;
    MainWindow* mainWindowPointer;
};

class QDoubleSpinBox;
class LabeledSlider;

class PhaseCorrectionWidget final : public ProcessingWidget
{
    Q_OBJECT

public:

    explicit PhaseCorrectionWidget(Spectrum* experiment, QWidget *parent = nullptr);

    void changeActiveExperiment(Spectrum* experiment) override;

private:
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> ph0;
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> ph1;
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> pivot;

private slots:
    void ph0Slot(double phase);
    void ph1Slot(double phase);
};

class QListWidget;

class ZeroFillingWidget final : public ProcessingWidget
{
    Q_OBJECT

public:

    explicit ZeroFillingWidget(Spectrum* experiment, QWidget *parent = nullptr);

private:
    QListWidget* list;


private slots:
    void zeroFillingSlot(int n);

};



#endif // WIDGETS_FOR_PROCESSING_H
