#include "mainwindow.h"

#include "file_io/general.h"
#include "spectrum/spectrum.h"

#include "gui/spectrumdisplayer.h"
#include "gui/widgets_for_processing.h"


#include <QScreen>
#include <QMenu>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QStackedWidget>
#include <QFrame>
#include <QLabel>
#include <QPen>
#include <QList>
#include <QDockWidget>
#include <QShortcut>
#include <QMessageBox>

#include <filesystem>
#include <type_traits>

void qt_set_sequence_auto_mnemonic(bool b);

namespace
{
    const QString readErrorMessage(ReadError s)
    {
        using enum ReadError;
        switch(s) {
            case unknownFailure: return QStringLiteral("unknown failure has occured when opening file");
            case unknownFormat: return QStringLiteral("uknown file format");
            case noParamsFile: return QStringLiteral("There is no file with params, in addition to fid");
            case invalidFid: return QStringLiteral("Error while reading fid file");
            case invalidProcpar: return QStringLiteral("Error while reading procpar file");
            case invalidAcqus: return QStringLiteral("Error while reading acqus file");
            case invalidJDF: return QStringLiteral("Error while reading jdf file");
        }
        assert(false);
    }

    constexpr std::array BUTTONS_NEEDING_SPECTRUM{zoomB, zoomResetB, integrateB, integralsResetB};
    constexpr std::array ACTIONS_NEEDING_SPECTRUM{phaseCorrectionA, zeroFillingA};
    consteval DisplayerAction buttonToDisplayerAction(ButtonNames n)
    {
        using enum DisplayerAction;
        switch (n) {
        case zoomB: return Zoom;
        case integrateB : return Integrate;
        default : return None;
        }
        assert(false);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentAction_{DisplayerAction::None}
    , mainStackedWidget(new QStackedWidget())    
    , tabWidget(new TabWidget(mainStackedWidget, this))
    , processingWidgets{}
    , currentAction{currentAction_}

{
    qt_set_sequence_auto_mnemonic(true);
    activateWindow();

    setWindowState(Qt::WindowMaximized);

    setWindowTitle(tr("Open NMR"));
    createActions();
    createTopMenuBar();
    createActionsFrame();
    createKeyShortcuts();

    QWidget* mainWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout();
    QVBoxLayout* rightSideLayout = new QVBoxLayout();

    rightSideLayout->addWidget(actionsFrame);
    rightSideLayout->addWidget(tabWidget);
    mainLayout->addWidget(mainStackedWidget);
    mainLayout->addLayout(rightSideLayout);
    mainWidget->setLayout(mainLayout);

    mainLayout->setStretchFactor(mainStackedWidget, 12);
    mainLayout->setStretchFactor(rightSideLayout, 1);

    setCentralWidget(mainWidget);

    connect(mainStackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::spectrumChangedSlot);
    connect(this, &MainWindow::displayedSpectrumChanged, this, [this](){emit closeDynamicElements();});

    // disabling things that need open spectrum
    for (auto i : ACTIONS_NEEDING_SPECTRUM) {actions[i]->setEnabled(false);}
    for (auto i : BUTTONS_NEEDING_SPECTRUM) {buttons[i]->setEnabled(false);}

}

MainWindow::~MainWindow()
{
}

const QString MainWindow::actionToName(ActionNames a)
{
    switch (a) {
    case openFileA: return QStringLiteral("Open File");
    case zoomA: return QStringLiteral("Zoom");
    case zoomResetA: return QStringLiteral("Reset Zoom");
    case integrateA: return QStringLiteral("Integrate");
    case integralsResetA: return QStringLiteral("Reset Integrals");
    case closeAppA: return QStringLiteral("Close App");
    case phaseCorrectionA: return QStringLiteral("Phase Correction");
    case zeroFillingA: return QStringLiteral("Zero Filling/Truncation");
    default:
    assert(false && "Not covered enum value in:" && __func__);
    }
}

void MainWindow::createActions()
{
    // makeAction creates action in actions array with position specified by ActionNames and text by actionToName,
    // connects it to slot passed as second argument
    auto makeAction = [this](ActionNames a, auto slot)
    {
        actions[a] = new QAction(actionToName(a), this);
        connect(actions[a], &QAction::triggered, this, slot);
    };

    makeAction(openFileA, &MainWindow::openFileSlot);
    makeAction(zoomA, &MainWindow::zoomSlot);
    makeAction(zoomResetA, &MainWindow::zoomResetSlot);
    makeAction(integrateA, &MainWindow::integrateSlot);
    makeAction(integralsResetA, &MainWindow::integralsResetSlot);
    makeAction(closeAppA, &MainWindow::close);
    makeAction(phaseCorrectionA, &MainWindow::phaseCorrectionSlot);
    makeAction(zeroFillingA, &MainWindow::zeroFillingSlot);

    // actions[openFileA] = new QAction(tr("Open File"), this);
    // connect(actions[openFileA], &QAction::triggered, this, &MainWindow::openFileSlot);

    // actions[closeAppA] = new QAction(tr("Close App"), this);
    // connect(actions[closeAppA], &QAction::triggered, this, &MainWindow::close);

    // actions[phaseCorrectionA] = new QAction(tr("Phase Correction"), this);
    // connect(actions[phaseCorrectionA], &QAction::triggered, this, &MainWindow::phaseCorrectionSlot);

    // actions[zeroFillingA] = new QAction(tr("Zero Filling"), this);
    // connect(actions[zeroFillingA], &QAction::triggered, this, &MainWindow::zeroFillingSlot);
}

void MainWindow::createTopMenuBar()
{
    topMenuBar = menuBar();
    connect(topMenuBar, &QMenuBar::triggered, this, &MainWindow::finishAction);
    QMenu* fileMenu = topMenuBar->addMenu(tr("File"));
    fileMenu->addAction(actions[openFileA]);
    fileMenu->addAction(actions[closeAppA]);

    QMenu* processingMenu = topMenuBar->addMenu(tr("Processing"));
    processingMenu->addAction(actions[phaseCorrectionA]);
    processingMenu->addAction(actions[zeroFillingA]);
}

void MainWindow::createActionsFrame()
{
    actionsFrame = new QFrame(this);
    QVBoxLayout* actionsLayout = new QVBoxLayout(actionsFrame);

    buttons[openFileB] = new QPushButton(tr("Open File"), actionsFrame);
    connect(buttons[openFileB], &QPushButton::clicked, this, &MainWindow::openFileSlot);


    buttons[zoomB] = new QPushButton(tr("Zoom"), actionsFrame);
    buttons[zoomB]->setCheckable(true);
    connect(buttons[zoomB], &QPushButton::clicked, this, &MainWindow::zoomSlot);

    buttons[zoomResetB] = new QPushButton(tr("Reset Zoom"), actionsFrame);
    connect(buttons[zoomResetB], &QPushButton::clicked, this, &MainWindow::zoomResetSlot);

    buttons[integrateB] = new QPushButton(tr("Integrate"), actionsFrame);
    buttons[integrateB]->setCheckable(true);
    connect(buttons[integrateB], &QPushButton::clicked, this, &MainWindow::integrateSlot);

    buttons[integralsResetB] = new QPushButton(tr("Reset Integrals"), actionsFrame);
    connect(buttons[integralsResetB], &QPushButton::clicked, this, &MainWindow::integralsResetSlot);

    actionsLayout->addWidget(buttons[openFileB]);
    actionsLayout->addWidget(buttons[zoomB]);
    actionsLayout->addWidget(buttons[zoomResetB]);
    actionsLayout->addWidget(buttons[integrateB]);
    actionsLayout->addWidget(buttons[integralsResetB]);

    actionsFrame->setLayout(actionsLayout);
}

void MainWindow::createKeyShortcuts()
{
    // Z - zoom
    auto zoomShorcut = new QShortcut(Qt::Key_Z, this);
    connect(zoomShorcut, &QShortcut::activated, this, &MainWindow::zoomSlot);

    // F - reset zoom
    auto resetZoomShortcut = new QShortcut(Qt::Key_F, this);
    connect(resetZoomShortcut, &QShortcut::activated, this, &MainWindow::zoomResetSlot);

    // I - integrate
    auto integrateShortCut = new QShortcut(Qt::Key_I, this);
    connect(integrateShortCut, &QShortcut::activated, this, &MainWindow::integrateSlot);
}

//! slot responsible for opening files
void MainWindow::openFileSlot()
{
    QFileDialog fileDialog(this, tr("Open File"));
    fileDialog.exec();
    if (fileDialog.selectedFiles().isEmpty())
    {
        return;
    }
    QString selectedFile = fileDialog.selectedFiles().at(0);
    std::filesystem::path inputPath{selectedFile.toStdString()};
    ReadResult fileReadResult = open_experiment_(inputPath);

    // displaying errror message when fileReadResult indicates error
    if (not fileReadResult) {
        QMessageBox msg{};
        msg.setWindowTitle(QStringLiteral("Error"));
        msg.setText(readErrorMessage(fileReadResult.error()));
        msg.exec();
        return;
    }


    std::unique_ptr<Spectrum_1D> experiment = Spectrum_1D::uPtrFromReadResult(fileReadResult);

    SpectrumDisplayer_1D* spectrumDisplayer = new SpectrumDisplayer_1D(std::move(experiment), this);


    size_t i = mainStackedWidget->addWidget(spectrumDisplayer);
    mainStackedWidget->setCurrentIndex(i);
    tabWidget->addTab(spectrumDisplayer);

}

#define UNCHECK_ACTIVE_BUTTON() do {                \
if (buttons[ActiveButton]) {                        \
        buttons[ActiveButton]->setChecked(false); } \
    buttons[ActiveButton] = nullptr;                \
} while (0)

void MainWindow::finishAction()
{
    UNCHECK_ACTIVE_BUTTON();
    currentAction_ = DisplayerAction::None;
}

#define CHANGE_MUTUALLY_EXCLUSIVE_BUTTON(x)         \
do {                                                \
if (buttons[ActiveButton] == buttons[x]) {          \
    buttons[ActiveButton] = nullptr;                \
    buttons[x]->setChecked(false);                  \
    finishAction();                                 \
} else {                                            \
    if (buttons[ActiveButton]) {                    \
        buttons[ActiveButton]->setChecked(false);}  \
    buttons[ActiveButton] = buttons[x];             \
    buttons[x]->setChecked(true);                   \
    currentAction_ = buttonToDisplayerAction(x);    \
}                                                   \
} while (0)


void MainWindow::zoomSlot()
{
    CHANGE_MUTUALLY_EXCLUSIVE_BUTTON(zoomB);
    emit closeDynamicElements();
}


void MainWindow::zoomResetSlot()
{
    UNCHECK_ACTIVE_BUTTON();
    finishAction();
    if (mainStackedWidget->count() == 1) {return;}
    qobject_cast<SpectrumDisplayer_1D*>(mainStackedWidget->currentWidget())->resetZoom();
    emit closeDynamicElements();
}

void MainWindow::integrateSlot()
{
    CHANGE_MUTUALLY_EXCLUSIVE_BUTTON(integrateB);
    emit closeDynamicElements();
}

void MainWindow::integralsResetSlot()
{
    UNCHECK_ACTIVE_BUTTON();
    finishAction();
    if (mainStackedWidget->count() == 1) {return;}
        auto p = qobject_cast<SpectrumDisplayer_1D*>(mainStackedWidget->currentWidget());
        resetIntegrals(p->experiment->integrals);
        p->update();
    emit closeDynamicElements();
}

//! slot called when active widget in mainStackedWidget is changed
void MainWindow::spectrumChangedSlot(int i)
{
    UNCHECK_ACTIVE_BUTTON();
    finishAction();
    // checks if active widget is SpectrumDisplayer_1D or empty widget at the bottom of mainStackedWidget
    if (auto p = qobject_cast<SpectrumDisplayer_1D*>(mainStackedWidget->widget(i))) {
        // enabling actions and buttons needing spectrum
        for (auto i : ACTIONS_NEEDING_SPECTRUM) {actions[i]->setEnabled(true);}
        for (auto i : BUTTONS_NEEDING_SPECTRUM) {buttons[i]->setEnabled(true);}
        emit displayedSpectrumChanged(p->experiment.get());
        emit closeDynamicElements(); // closing open dock widgets
    } else { // no spectrum visible, disabling action and buttons needing spectrum
        for (auto i : ACTIONS_NEEDING_SPECTRUM) {actions[i]->setEnabled(false);}
        for (auto i : BUTTONS_NEEDING_SPECTRUM) {buttons[i]->setEnabled(false);}
        emit allSpectraClosed();
    }
}

template<typename T>
void MainWindow::showProcessingWidget()
{
    if (mainStackedWidget->count() == 1) {return;}
    if (auto& dockWidget = std::get<QDockWidget*>(std::get<std::tuple<T*, QDockWidget*>>(processingWidgets));
    not dockWidget) {
        auto& processingWidget = std::get<T*>(std::get<std::tuple<T*, QDockWidget*>>(processingWidgets));
        processingWidget = new T(qobject_cast<SpectrumDisplayer_1D*>(mainStackedWidget->currentWidget())->experiment.get(), this);
        dockWidget = new QDockWidget(this);
        dockWidget->setWidget(processingWidget);
        // hides processing widgets when all spectra are closed
        connect(this, &MainWindow::allSpectraClosed, dockWidget, &QDockWidget::hide);

        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    } else {
        dockWidget->show();

    }
    emit closeDynamicElements();
}

void MainWindow::phaseCorrectionSlot()
{
    showProcessingWidget<PhaseCorrectionWidget>();
}

void MainWindow::zeroFillingSlot()
{
    showProcessingWidget<ZeroFillingWidget>();
}

void MainWindow::refreshCurrentDisplayerSlot()
{
    mainStackedWidget->currentWidget()->update();
    emit closeDynamicElements();
}


