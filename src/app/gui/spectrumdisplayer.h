#ifndef SPECTRUMDISPLAYER_H
#define SPECTRUMDISPLAYER_H


#include <QObject>
#include <QWidget>
#include <QPointF>

class Spectrum;
class MainWindow;
class SpectrumPainter;
class XAxis;
class IntegralsDisplayer;

// widget that governs fully display of spectrum
//
// layout:
//
// SpectrumPainter    |
// XAxis              | YAxis
// IntegralsDisplayer |
class SpectrumDisplayer : public QWidget
{
    Q_OBJECT
public:
    SpectrumDisplayer(std::unique_ptr<Spectrum>&& new_experiment, QWidget* parent = nullptr);
    ~SpectrumDisplayer();

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

    std::unique_ptr<Spectrum> experiment;
    void resetZoom();

    void updateAll();


private:
    SpectrumPainter* spainter;
    XAxis* xAxis;
    IntegralsDisplayer* idisplayer;

    QPointF mouseMoveStartPoint;
    QPointF mouseMoveEndPoint;

    MainWindow* const mainWindow;


};

#endif // SPECTRUMDISPLAYER_H
