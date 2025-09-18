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
    QString readErrorMessage(ReadStatus s)
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
    #ifdef DEBUG__
        qDebug() << "DEBUG__";
    #endif

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

}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    openFileAction = new QAction(tr("Open File"), this);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFileSlot);
    closeAppAction = new QAction(tr("Close App"), this);
    connect(closeAppAction, &QAction::triggered, this, &MainWindow::close);
    phaseCorrectionAction = new QAction(tr("Phase Correction"), this);
    connect(phaseCorrectionAction, &QAction::triggered, this, &MainWindow::phaseCorrectionSlot);
    zeroFillingAction = new QAction(tr("Zero Filling"), this);
    connect(zeroFillingAction, &QAction::triggered, this, &MainWindow::zeroFillingSlot);
}

void MainWindow::createTopMenuBar()
{
    topMenuBar = menuBar();

    QMenu* fileMenu = topMenuBar->addMenu(tr("File"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(closeAppAction);

    QMenu* processingMenu = topMenuBar->addMenu(tr("Processing"));
    processingMenu->addAction(phaseCorrectionAction);
    processingMenu->addAction(zeroFillingAction);
}

void MainWindow::createActionsFrame()
{
    actionsFrame = new QFrame(this);
    QVBoxLayout* actionsLayout = new QVBoxLayout(actionsFrame);

    openFileButton = new QPushButton(tr("Open File"), actionsFrame);
    connect(openFileButton, &QPushButton::clicked, this, &MainWindow::openFileSlot);


    zoomButton = new QPushButton(tr("Zoom"), actionsFrame);
    zoomButton->setCheckable(true);
    connect(zoomButton, &QPushButton::clicked, this, &MainWindow::zoomSlot);

    zoomResetButton = new QPushButton(tr("Reset Zoom"), actionsFrame);
    connect(zoomResetButton, &QPushButton::clicked, this, &MainWindow::resetZoomSlot);

    integrateButton = new QPushButton(tr("&Integrate"), actionsFrame);
    integrateButton->setCheckable(true);
    connect(integrateButton, &QPushButton::clicked, this, &MainWindow::integrateSlot);

    resetIntegralsButton = new QPushButton(tr("Reset Integrals"), actionsFrame);
    connect(resetIntegralsButton, &QPushButton::clicked, this, &MainWindow::resetIntegralsSlot);

    actionsLayout->addWidget(openFileButton);
    actionsLayout->addWidget(zoomButton);
    actionsLayout->addWidget(zoomResetButton);
    actionsLayout->addWidget(integrateButton);
    actionsLayout->addWidget(resetIntegralsButton);

    actionsFrame->setLayout(actionsLayout);
}

void MainWindow::createKeyShortcuts()
{
    // Z - zoom
    auto zoomShorcut = new QShortcut(Qt::Key_Z, this);
    connect(zoomShorcut, &QShortcut::activated, this, [this](){
        if (zoomButton->isChecked()) {
            setCurrentAction(DisplayerAction::None);
        } else {
            zoomButton->setChecked(true);
            zoomSlot();
        }
        emit closeDynamicElements();
    });

    // F - reset zoom
    auto resetZoomShortcut = new QShortcut(Qt::Key_F, this);
    connect(resetZoomShortcut, &QShortcut::activated, this, &MainWindow::resetZoomSlot);

    // I - integrate
    auto integrateShortCut = new QShortcut(Qt::Key_I, this);
    connect(integrateShortCut, &QShortcut::activated, this, [this](){
        if (integrateButton->isChecked()) {
            setCurrentAction(DisplayerAction::None);
        } else {
            integrateButton->setChecked(true);
            integrateSlot();
        }
        emit closeDynamicElements();
    });
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

    if (not (file_read_result.status == ReadStatus::success_1D)) {
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

void MainWindow::setCurrentAction(DisplayerAction action)
{
    using enum DisplayerAction;
    if (currentAction_ == action) {return;}

    switch(currentAction_)
    {
    case Zoom:
        zoomButton->setChecked(false);
        break;
    case Integrate:
        integrateButton->setChecked(false);
        break;
    case None:
        break;
    }

    currentAction_ = action;

    if (mainStackedWidget->count() == 1) {
        setCurrentAction(None);
    }
}

void MainWindow::zoomSlot()
{
    setCurrentAction(DisplayerAction::Zoom);
    emit closeDynamicElements();
}

void MainWindow::resetZoomSlot()
{
    setCurrentAction(DisplayerAction::None);
    if (mainStackedWidget->count() == 1) {return;}
    qobject_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget())->resetZoom();
    emit closeDynamicElements();
}

void MainWindow::integrateSlot()
{
    setCurrentAction(DisplayerAction::Integrate);
    emit closeDynamicElements();
}

void MainWindow::resetIntegralsSlot()
{
    setCurrentAction(DisplayerAction::None);
    if (mainStackedWidget->count() == 1) {return;}
        auto p = qobject_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget());
        resetIntegrals(p->experiment->integrals);
        p->update();
    emit closeDynamicElements();
}

void MainWindow::spectrumChangedSlot(int i)
{
    if (auto p = qobject_cast<SpectrumDisplayer*>(mainStackedWidget->widget(i))) {
        emit displayedSpectrumChanged(p->experiment.get());
        emit closeDynamicElements();
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


