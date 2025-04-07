#ifndef INTEGRALSDISPLAYER_H
#define INTEGRALSDISPLAYER_H

#include <QObject>
#include <QWidget>
#include <QPen>

class Spectrum;

class IntegralsDisplayer : public QWidget
{
    Q_OBJECT
public:
    explicit IntegralsDisplayer(const Spectrum* integrals, QWidget *parent);

    void setRange(size_t begin, size_t end);

    void paintEvent(QPaintEvent* e) override;

private:
    const Spectrum* experiment;
    size_t startPoint_;
    size_t endPoint_;
    size_t currentSpectrumSize;
    QPen pen;


    void recalculateDisplayRange();


};

#endif // INTEGRALSDISPLAYER_H
