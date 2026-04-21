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
    // SpectrumPainter (idisplayer as overlay)   | YAxis
    // XAxis                                     | -----
    //                                           | placeholder
    Q_OBJECT
public:
    SpectrumDisplayer_1D(std::unique_ptr<Spectrum_1D>&& new_experiment, QWidget* parent);
    ~SpectrumDisplayer_1D();

    friend class IntegralsDisplayer;

    static SpectrumDisplayer_1D* findFrom(QWidget* widget) {
        while (widget) {
            SpectrumDisplayer_1D* foo = qobject_cast<SpectrumDisplayer_1D*>(widget);
            if (foo) {
                return foo;
            }
            widget = widget->parentWidget();
        }
        return nullptr;
    }

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

    std::unique_ptr<Spectrum_1D> experiment;
    void resetZoom() override;

    //! correct order of updating children widgets, solves problems with blinking when idisplayer is updated
    void updateAll() override;


private:
    SpectrumPainter* spainter;
    UniversalAxis* xAxis;
    UniversalAxis* yAxis;
    IntegralsDisplayer* idisplayer;

    QPointF mouseMoveStartPoint;
    QPointF mouseMoveEndPoint;

    MainWindow* const mainWindow;

private slots:
    //! function responsible for right click context menu
    void showContextMenu(const QPoint &pos);

};

#endif // SPECTRUMDISPLAYER_H
