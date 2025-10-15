#ifndef SPECTRUMDISPLAYER_H
#define SPECTRUMDISPLAYER_H


#include <QObject>
#include <QWidget>
#include <QPointF>

class Spectrum_1D;
class MainWindow;
class SpectrumPainter;
class XAxis;
class IntegralsDisplayer;
class UniversalAxis;
struct SpectrumInfo;
struct FidSizeInfo;
struct Phase;

enum class DisplayerAction{None, Zoom, Integrate};

//! interface for spectrum displayers
class ASpectrumDisplayer : public QWidget
{
    Q_OBJECT
public:
    ASpectrumDisplayer(const SpectrumInfo& info, const FidSizeInfo& fidInfo, QWidget* parent);
    virtual ~ASpectrumDisplayer();
    virtual void resetZoom() = 0;
    virtual void updateAll() = 0;

    const SpectrumInfo& info;
    const FidSizeInfo& fidInfo;

};

//! widget that governs fully display of spectrum
class SpectrumDisplayer_1D final : public ASpectrumDisplayer
{
    // layout:
    //
    // SpectrumPainter    | YAxis
    // XAxis              | -----
    // IntegralsDisplayer | placeholder
    Q_OBJECT
public:
    SpectrumDisplayer_1D(std::unique_ptr<Spectrum_1D>&& new_experiment, QWidget* parent);
    ~SpectrumDisplayer_1D();

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

    std::unique_ptr<Spectrum_1D> experiment;
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
