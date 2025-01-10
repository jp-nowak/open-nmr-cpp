#ifndef SPECTRUMDISPLAYER_H
#define SPECTRUMDISPLAYER_H

#include "spectrumpainter.h"
#include "xaxis.h"
#include "spectrum_displayer_actions.h"


#include <QObject>
#include <QWidget>
#include <QPointF>


class SpectrumDisplayer : public QWidget
{
    Q_OBJECT
public:
    SpectrumDisplayer(std::unique_ptr<Spectrum>&& new_experiment, QWidget* parent = nullptr);

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

    DisplayerAction currentAction;
    std::unique_ptr<Spectrum> experiment;
    void resetZoom();
private:
    SpectrumPainter* spainter;
    XAxis* xAxis;

    QPointF mouseMoveStartPoint;
    QPointF mouseMoveEndPoint;




};

#endif // SPECTRUMDISPLAYER_H
