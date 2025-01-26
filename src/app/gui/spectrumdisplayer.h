#ifndef SPECTRUMDISPLAYER_H
#define SPECTRUMDISPLAYER_H

#include "spectrumpainter.h"
#include "xaxis.h"


#include <QObject>
#include <QWidget>
#include <QPointF>

class MainWindow;


class SpectrumDisplayer : public QWidget
{
    Q_OBJECT
public:
    SpectrumDisplayer(std::unique_ptr<Spectrum>&& new_experiment, QWidget* parent = nullptr);

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

    std::unique_ptr<Spectrum> experiment;
    void resetZoom();
private:
    SpectrumPainter* spainter;
    XAxis* xAxis;

    QPointF mouseMoveStartPoint;
    QPointF mouseMoveEndPoint;

    MainWindow* const mainWindow;


};

#endif // SPECTRUMDISPLAYER_H
