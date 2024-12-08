#ifndef SPECTRUMDISPLAYER_H
#define SPECTRUMDISPLAYER_H

#include "spectrumpainter.h"
#include "xaxis.h"

#include <QObject>
#include <QWidget>



class SpectrumDisplayer : public QWidget
{
    Q_OBJECT
public:
    SpectrumDisplayer(SpectrumPainter* spainter, QWidget* parent = nullptr);


private:
    SpectrumPainter* spainter;
    XAxis* xAxis;


};

#endif // SPECTRUMDISPLAYER_H
