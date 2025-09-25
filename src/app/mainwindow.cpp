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

void qt_set_sequence_auto_mnemonic(bool b);

namespace
{

    const QString readErrorMessage(ReadStatus s)
    {
        using enum ReadStatus;
        switch (s) {
            case unknown_failure: return QStringLiteral("unknown failure has occured when opening file");
            case success_1D: return QStringLiteral("1D experiment opened successfuly");
            case success_2D: return QStringLiteral("2D experiments are not supported yet");
            case unknown_format: return QStringLiteral("uknown file format");
            case invalid_fid: return QStringLiteral("fid file is corrupted");
            case invalid_procpar: return QStringLiteral("procpar file is corrupted");
            case invalidAcqus: return QStringLiteral("acqus file is corrupted");
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

void MainWindow::createActions()
{
    actions[openFileA] = new QAction(tr("Open File"), this);
    connect(actions[openFileA], &QAction::triggered, this, &MainWindow::openFileSlot);

    actions[closeAppA] = new QAction(tr("Close App"), this);
    connect(actions[closeAppA], &QAction::triggered, this, &MainWindow::close);

    actions[phaseCorrectionA] = new QAction(tr("Phase Correction"), this);
    connect(actions[phaseCorrectionA], &QAction::triggered, this, &MainWindow::phaseCorrectionSlot);

    actions[zeroFillingA] = new QAction(tr("Zero Filling"), this);
    connect(actions[zeroFillingA], &QAction::triggered, this, &MainWindow::zeroFillingSlot);
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
    connect(buttons[zoomResetB], &QPushButton::clicked, this, &MainWindow::resetZoomSlot);

    buttons[integrateB] = new QPushButton(tr("Integrate"), actionsFrame);
    buttons[integrateB]->setCheckable(true);
    connect(buttons[integrateB], &QPushButton::clicked, this, &MainWindow::integrateSlot);

    buttons[integralsResetB] = new QPushButton(tr("Reset Integrals"), actionsFrame);
    connect(buttons[integralsResetB], &QPushButton::clicked, this, &MainWindow::resetIntegralsSlot);

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
    connect(resetZoomShortcut, &QShortcut::activated, this, &MainWindow::resetZoomSlot);

    // I - integrate
    auto integrateShortCut = new QShortcut(Qt::Key_I, this);
    connect(integrateShortCut, &QShortcut::activated, this, &MainWindow::integrateSlot);
}

void MainWindow::openFileSlot()
{
    QFileDialog fileDialog(this, tr("Open File"));
    fileDialog.exec();
    if (fileDialog.selectedFiles().isEmpty())
    {
        return;
    }
    QString selectedFile = fileDialog.selectedFiles().at(0);
    std::filesystem::path input_path{selectedFile.toStdString()};
    FileReadResult file_read_result = open_experiment(input_path);

    if (file_read_result.status != ReadStatus::success_1D) {
        QMessageBox msg{};
        msg.setWindowTitle(QStringLiteral("Error"));
        msg.setText(readErrorMessage(file_read_result.status));
        msg.exec();
        return;
    }

    std::unique_ptr<Spectrum> experiment = Spectrum::pointer_from_file_read_result(file_read_result);

    SpectrumDisplayer* spectrumDisplayer = new SpectrumDisplayer(std::move(experiment), this);


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


void MainWindow::resetZoomSlot()
{
    UNCHECK_ACTIVE_BUTTON();
    finishAction();
    if (mainStackedWidget->count() == 1) {return;}
    qobject_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget())->resetZoom();
    emit closeDynamicElements();
}

void MainWindow::integrateSlot()
{
    CHANGE_MUTUALLY_EXCLUSIVE_BUTTON(integrateB);
    emit closeDynamicElements();
}

void MainWindow::resetIntegralsSlot()
{
    UNCHECK_ACTIVE_BUTTON();
    finishAction();
    if (mainStackedWidget->count() == 1) {return;}
        auto p = qobject_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget());
        resetIntegrals(p->experiment->integrals);
        p->update();
    emit closeDynamicElements();
}

//! slot called when active widget in mainStackedWidget is changed
void MainWindow::spectrumChangedSlot(int i)
{
    UNCHECK_ACTIVE_BUTTON();
    finishAction();
    // checks if active widget is SpectrumDisplayer or empty widget at the bottom of mainStackedWidget
    if (auto p = qobject_cast<SpectrumDisplayer*>(mainStackedWidget->widget(i))) {
        // enabling actions and buttons needing spectrum
        for (auto i : ACTIONS_NEEDING_SPECTRUM) {actions[i]->setEnabled(true);}
        for (auto i : BUTTONS_NEEDING_SPECTRUM) {buttons[i]->setEnabled(true);}
        emit displayedSpectrumChanged(p->experiment.get());
        emit closeDynamicElements(); // closing open dock widgets
    } else { // no spectrum visible, disabling action and buttons needing spectrum
        for (auto i : ACTIONS_NEEDING_SPECTRUM) {actions[i]->setEnabled(false);}
        for (auto i : BUTTONS_NEEDING_SPECTRUM) {buttons[i]->setEnabled(false);}
    }
}

template<typename T>
void MainWindow::showProcessingWidget()
{
    if (mainStackedWidget->count() == 1) {return;}
    if (auto& dockWidget = std::get<QDockWidget*>(std::get<std::tuple<T*, QDockWidget*>>(processingWidgets));
    not dockWidget) {
        auto& processingWidget = std::get<T*>(std::get<std::tuple<T*, QDockWidget*>>(processingWidgets));
        processingWidget = new T(qobject_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget())->experiment.get(), this);
        dockWidget = new QDockWidget(this);
        dockWidget->setWidget(processingWidget);
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
        dockWidget->resize(0, 0);
    } else {
        dockWidget->show();
        dockWidget->resize(0, 0);
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
    reinterpret_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget())->update();
    emit closeDynamicElements();
}


