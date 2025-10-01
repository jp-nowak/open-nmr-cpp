#ifndef WIDGETS_FOR_PROCESSING_H
#define WIDGETS_FOR_PROCESSING_H

#include <QObject>
#include <QWidget>

class Spectrum_1D;
class MainWindow;

class ProcessingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessingWidget(Spectrum_1D* experiment, QWidget *parent = nullptr);

public slots:
    virtual void changeActiveExperiment(Spectrum_1D* experiment) = 0;

signals:
    void signalToRefreshDisplayedExperiment();

protected:
    Spectrum_1D* experiment;
    MainWindow* mainWindowPointer;
};

//------------------------------------------------------------------------------------------------------------------------

class QDoubleSpinBox;
class LabeledSlider;

class PhaseCorrectionWidget final : public ProcessingWidget
{
    Q_OBJECT

public:

    explicit PhaseCorrectionWidget(Spectrum_1D* experiment, QWidget *parent = nullptr);

    void changeActiveExperiment(Spectrum_1D* experiment) override;

private:
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> ph0;
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> ph1;
    std::tuple<QWidget*, LabeledSlider*, QDoubleSpinBox*> pivot;

private slots:
    void ph0Slot(double phase);
    void ph1Slot(double phase);
};

//------------------------------------------------------------------------------------------------------------------------

class QListWidget;
class QSpinBox;
class QLabel;

class ZeroFillingWidget final : public ProcessingWidget
{
    Q_OBJECT

public:

    explicit ZeroFillingWidget(Spectrum_1D* experiment, QWidget *parent = nullptr);

    void changeActiveExperiment(Spectrum_1D* experiment) override;

private:
    QListWidget* list;
    QSpinBox* spinBox;
    QLabel* sizeLabel;

private slots:
    void zeroFillingSlot(int n);
    void truncatingSlot(int n);
};



#endif // WIDGETS_FOR_PROCESSING_H
