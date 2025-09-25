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
class UniversalAxis;
struct SpectrumInfo;

enum class DisplayerAction{None, Zoom, Integrate};

//! interface for spectrum displayers
class ASpectrumDisplayer : public QWidget
{
    Q_OBJECT
public:
    ASpectrumDisplayer(const SpectrumInfo& info, QWidget* parent);
    virtual ~ASpectrumDisplayer();
    virtual void resetZoom() = 0;
    virtual void updateAll() = 0;

    const SpectrumInfo& info;

};

//! widget that governs fully display of spectrum
class SpectrumDisplayer final : public ASpectrumDisplayer
{
    // layout:
    //
    // SpectrumPainter    | YAxis
    // XAxis              | -----
    // IntegralsDisplayer | placeholder
    Q_OBJECT
public:
    SpectrumDisplayer(std::unique_ptr<Spectrum>&& new_experiment, QWidget* parent);
    ~SpectrumDisplayer();

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

    std::unique_ptr<Spectrum> experiment;
    void resetZoom() override;

    void updateAll() override;


private:
    SpectrumPainter* spainter;
    UniversalAxis* xAxis;
    UniversalAxis* yAxis;
    IntegralsDisplayer* idisplayer;

    QPointF mouseMoveStartPoint;
    QPointF mouseMoveEndPoint;

    MainWindow* const mainWindow;


};

#endif // SPECTRUMDISPLAYER_H
