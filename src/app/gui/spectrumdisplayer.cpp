#include "spectrumdisplayer.h"

#include "../mainwindow.h"
#include "gui_utilities.h"
#include "spectrumpainter.h"
#include "xaxis.h"
#include "integralsdisplayer.h"

#include <cassert>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QMenu>
#include <QActionGroup>

namespace
{
QColor mapDisplayerActionToColor(DisplayerAction action)
{
    using enum DisplayerAction;
    switch(action)
    {
    case Zoom:
        return QColor(255, 0, 0, 100); // RED
    case Integrate:
        return QColor(0, 255, 0, 100); // GREEN
    case None:
        break;
    }
    return QColor(0, 0, 0, 100);
}

constexpr double idisplayerPos = 0.85;
constexpr double idisplayerHeight = 0.2;

constexpr double kHz_to_Hz = 1000;

constexpr std::array ALLOWED_X_UNITS_SPECTRUM_1D{XU_ppm, XU_Points, XU_Hz, XU_kHz};
constexpr std::array ALLOWED_X_UNITS_FID_1D{XU_Seconds, XU_Points};

}

QString UnitsToString(XAxisUnits x)
{
    switch (x) {
    case XU_ppm:     return QStringLiteral("ppm");     break;
    case XU_Points:  return QStringLiteral("Points");  break;
    case XU_Hz:      return QStringLiteral("Hz");      break;
    case XU_kHz:     return QStringLiteral("kHz");     break;
    case XU_Seconds: return QStringLiteral("Seconds"); break;
    default:
        assert(false);
    }
}

ASpectrumDisplayer::ASpectrumDisplayer(const SpectrumInfo& info, const FidSizeInfo& fidInfo, QWidget* parent)
: QWidget{parent}
, info{info}
, fidInfo{fidInfo}
{}

ASpectrumDisplayer::~ASpectrumDisplayer()
{

}

SpectrumDisplayer_1D::SpectrumDisplayer_1D(std::unique_ptr<Spectrum_1D>&& new_experiment, QWidget* parent)
: ASpectrumDisplayer{new_experiment->info, new_experiment->getFidSizeInfo(), parent}
, experiment{std::move(new_experiment)}
, spainter{new SpectrumPainter{experiment.get(), this}}
, xAxisUnit{XU_ppm}
, idisplayer{new IntegralsDisplayer{experiment.get(), this}}
, displayedElement{DisplayedElement::Spectrum1}
, mouseMoveStartPoint{0, 0}
, currentZoom{}
, mainWindow{MainWindow::findFrom(this)}
{
    assert(mainWindow && "nullptr to main window");
    assert(parent && "nullptr to parent");

    // QVBoxLayout* spectrumAndXAxis = new QVBoxLayout();

    auto& info = experiment->info;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &SpectrumDisplayer_1D::customContextMenuRequested, this, &SpectrumDisplayer_1D::showContextMenu);

    xAxis = new UniversalAxis{AxisProperties{
                .minimum = info.plot_right_ppm,
                .maximum = info.plot_left_ppm,
                .fontSize = 15,
                .showLine = true,
                .vertical = false,
                .descending = true,
                .dynamic = true,
                .linePos = 0.1,
                .labelAdditionalSpacing = 0.01},
                              this};


    yAxis = new UniversalAxis{AxisProperties{
            .minimum = -(spainter->baselinePosition * spainter->maximum / (1 - spainter->baselinePosition)),
            .maximum = spainter->maximum,
            .fontSize = 15,
            .showLine = true,
            .vertical = true,
            .descending = true,
            .dynamic = true,
            .relTickLine = 0.1,
            .linePos = 0.1,
            .labelAdditionalSpacing = 0.1
                        }, this};
    // QLabel* yAxis = new QLabel(tr("Y"), this);
    // yAxis->setAlignment(Qt::AlignVCenter);

    // rescaling of yAxis when spainter is scrolled
    connect(spainter, &SpectrumPainter::wheelTurned, this, [this](double x){
        auto [maximum, minimum] = yAxis->getRange();
        if (x > 0) {
            yAxis->setRange(minimum / x, maximum / x);
        } else {
            x = std::fabs(x);
            yAxis->setRange(minimum * x, maximum * x);
        }
        yAxis->update();
    });

    // painting integral displayer causes spectrum painter to be repainted, otherwise there is a blank space in borders of integral displayer
    connect(idisplayer, &IntegralsDisplayer::updated, this, [this](){spainter->update(); xAxis->update();});

    connect(mainWindow, &MainWindow::closeDynamicElements, this, [this](){spainter->resetSelection(); updateAll();});

    rightBottomEdge = new QLabel(UnitsToString(xAxisUnit), this);
    rightBottomEdge->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    auto* layout = new QGridLayout();
    layout->addWidget(spainter, 0, 0);
    auto* xAxisLayout = new QVBoxLayout();
    // xAxisLayout->addWidget(idisplayer);
    xAxisLayout->addWidget(xAxis);
    // xAxisLayout->setStretchFactor(idisplayer, 1);
    xAxisLayout->setStretchFactor(xAxis, 2);

    idisplayer->show();
    idisplayer->move(spainter->geometry().x(), spainter->height() * idisplayerPos);
    idisplayer->resize(spainter->width(), spainter->height() * idisplayerHeight);

    layout->addLayout(xAxisLayout, 1, 0);
    layout->addWidget(yAxis, 0, 1);
    layout->addWidget(rightBottomEdge, 1, 1);
    layout->setColumnStretch(0, 15);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(0, 14);
    layout->setRowStretch(1, 2);
    setLayout(layout);

    // spectrumAndXAxis->addWidget(spainter);
    // spectrumAndXAxis->addWidget(idisplayer);
    // spectrumAndXAxis->addWidget(xAxis);
    // spectrumAndXAxis->setStretchFactor(spainter, 12);
    // spectrumAndXAxis->setStretchFactor(idisplayer, 1);
    // spectrumAndXAxis->setStretchFactor(xAxis, 1);

    // QHBoxLayout* spectrumWithXAxisAndYAxis = new QHBoxLayout();
    // spectrumWithXAxisAndYAxis->addLayout(spectrumAndXAxis);
    // spectrumWithXAxisAndYAxis->addWidget(yAxis);
    // spectrumWithXAxisAndYAxis->setStretchFactor(spectrumAndXAxis, 60);
    // spectrumWithXAxisAndYAxis->setStretchFactor(yAxis, 1);

    // setLayout(spectrumWithXAxisAndYAxis);

}

SpectrumDisplayer_1D::~SpectrumDisplayer_1D()
{

}

void SpectrumDisplayer_1D::mousePressEvent(QMouseEvent* e)
{
    mouseMoveStartPoint = e->pos();

    // checks if mouse was clicked on part of widget not overlapping with idisplayer
    if (not idisplayer->rect().contains(idisplayer->mapFromGlobal(mapToGlobal(e->pos())))) {
        idisplayer->closeIntegralEditField(); // stops editing of integral
    }

    if (mainWindow->currentAction == DisplayerAction::None) return;

    spainter->resetSelection();
    spainter->setSelectionStart(mapToGlobal(mouseMoveStartPoint), mapDisplayerActionToColor(mainWindow->currentAction));
}

void SpectrumDisplayer_1D::mouseReleaseEvent(QMouseEvent* e)
{
    mouseMoveEndPoint = e->pos();

    using enum DisplayerAction;

    // double clicking on spectrum shall have no effect
    if (mouseMoveStartPoint.x() == mouseMoveEndPoint.x()) {
        mainWindow->finishAction();
        spainter->resetSelection();
        return;
    }

    switch (mainWindow->currentAction)
    {
    case Zoom:
        spainter->resetSelection();
        if (spainter->zoom(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint))) {
            xAxis->setRangePoints(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
            idisplayer->zoom(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
            currentZoom = {mouseMoveStartPoint, mouseMoveEndPoint};
        }
        mainWindow->finishAction();
        break;

    case Integrate:
    {
        spainter->resetSelection();
        auto [left, right] = spainter->selectionRangeToDataPointsOfSpectrum(mapToGlobal(mouseMoveStartPoint), mapToGlobal(mouseMoveEndPoint));
        experiment->integrate(left, right);
        mainWindow->finishAction();
        idisplayer->update();
        break;
    }
    case None:
        return;
    }
}

void SpectrumDisplayer_1D::mouseMoveEvent(QMouseEvent* e)
{
    if (mainWindow->currentAction == DisplayerAction::None) return;
    spainter->changeSelectionWidth(mapToGlobal(e->pos()), mapToGlobal(mouseMoveStartPoint));
}

void SpectrumDisplayer_1D::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    idisplayer->move(spainter->geometry().x(), spainter->height() * idisplayerPos);
    idisplayer->resize(spainter->width(), spainter->height() * idisplayerHeight);
}

void SpectrumDisplayer_1D::resetZoom()
{
    idisplayer->resetZoom();
    spainter->resetZoom();
    currentZoom = {{spainter->geometry().topLeft()}, {spainter->geometry().bottomRight()}};
    changeXAxisUnit(xAxisUnit);
}

void SpectrumDisplayer_1D::updateAll()
{
    idisplayer->update(); // causes update of spainter and xaxis as well
    //spainter->update();
    //xAxis->update();
}

void SpectrumDisplayer_1D::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    qDebug() << pos;
    MainWindow* mainWindow = MainWindow::findFrom(this);

    assert(mainWindow && "nullptr to main window");
    if (not mainWindow) return;

    contextMenu.addAction(mainWindow->actions[zoomA]);
    contextMenu.addAction(mainWindow->actions[zoomResetA]);
    contextMenu.addAction(mainWindow->actions[integrateA]);
    contextMenu.addAction(mainWindow->actions[integralsResetA]);
    qDebug() << childAt(pos);
    if ((childAt(pos) == xAxis) or (childAt(pos) == rightBottomEdge)) {
        qDebug() << "aa";
        QActionGroup units(this);
        units.setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
        QMenu* unitsMenu = contextMenu.addMenu(QStringLiteral("Units"));

        switch (displayedElement) {
        case DisplayedElement::Fid1:

            for (auto i : ALLOWED_X_UNITS_FID_1D) {
                QAction* a = new QAction(UnitsToString(i), this);
                connect(a, &QAction::triggered, this, [this, i](){this->changeXAxisUnit(i);});
                units.addAction(a);
                unitsMenu->addAction(a);
            }
            break;

        case DisplayedElement::Spectrum1:

            for (auto i : ALLOWED_X_UNITS_SPECTRUM_1D) {
                QAction* a = new QAction(UnitsToString(i), this);
                a->setCheckable(true);
                connect(a, &QAction::triggered, this, [this, i](){this->changeXAxisUnit(i);});
                if (i == xAxisUnit) a->setChecked(true);
                units.addAction(a);
                unitsMenu->addAction(a);
            }
            break;

        default: assert(false);
        }

    }

    contextMenu.exec(mapToGlobal(pos));

}

void SpectrumDisplayer_1D::changeXAxisUnit(XAxisUnits unit)
{
    switch (displayedElement) {

    case DisplayedElement::Fid1:
        assert(false);
        if (auto i = std::find(std::begin(ALLOWED_X_UNITS_FID_1D),
                std::end(ALLOWED_X_UNITS_FID_1D),
                unit); i == std::end(ALLOWED_X_UNITS_FID_1D)) return;
        break;

    case DisplayedElement::Spectrum1:
        if (auto i = std::find(std::begin(ALLOWED_X_UNITS_SPECTRUM_1D),
                std::end(ALLOWED_X_UNITS_SPECTRUM_1D),
                unit); i == std::end(ALLOWED_X_UNITS_SPECTRUM_1D)) return;
        break;

    }

    AxisProperties p = xAxis->properties();

    switch (unit) {

    case XU_ppm:
        p.minimum = info.plot_right_ppm;
        p.maximum = info.plot_left_ppm;
        p.descending = true;
        break;

    case XU_Points:
        p.minimum = 0;
        p.maximum = experiment->get_spectrum().size();
        p.descending = false;
        break;

    case XU_Hz:
        p.minimum = info.plot_right_Hz;
        p.maximum = info.plot_left_Hz;
        p.descending = true;
        break;

    case XU_kHz:
        p.minimum = info.plot_right_Hz / kHz_to_Hz;
        p.maximum = info.plot_left_Hz / kHz_to_Hz;
        p.descending = true;
        break;

    case XU_Seconds:
        assert(false);

    default: assert(false);

    }

    xAxis->change(p);



    rightBottomEdge->setText(UnitsToString(unit));
    xAxisUnit = unit;

    if (not (currentZoom.first.isNull() and currentZoom.second.isNull())) {
        xAxis->setRangePoints(mapToGlobal(currentZoom.first), mapToGlobal(currentZoom.second));
    }

    updateAll();

}
