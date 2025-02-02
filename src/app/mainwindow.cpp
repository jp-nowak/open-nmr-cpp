#include "mainwindow.h"

#include "file_io/general.h"
#include "spectrum/spectrum.h"
#include "gui/spectrumdisplayer.h"

#include <filesystem>

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







MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentAction_{DisplayerAction::None}
    , mainStackedWidget(new QStackedWidget())    
    , tabWidget(new TabWidget(mainStackedWidget, this))
    , currentAction{currentAction_}

{

    resize(screen()->availableGeometry().size() * 0.7);
    #ifdef DEBUG__
        qDebug() << "DEBUG__";
    #endif

    setWindowTitle(tr("Open NMR"));
    createActions();
    createTopMenuBar();
    createActionsFrame();

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
    // actions ..

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
}

void MainWindow::createTopMenuBar()
{
    topMenuBar = menuBar();

    fileMenu = topMenuBar->addMenu(tr("File"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(closeAppAction);
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

    integrateButton = new QPushButton(tr("Integrate"), actionsFrame);

    actionsLayout->addWidget(openFileButton);
    actionsLayout->addWidget(zoomButton);
    actionsLayout->addWidget(zoomResetButton);
    actionsLayout->addWidget(integrateButton);

    actionsFrame->setLayout(actionsLayout);
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
    FileIO::FileReadResult file_read_result = FileIO::open_experiment(input_path);

    if (not (file_read_result.file_read_status == FileIO::FileReadStatus::success_1D)) {
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
    if (currentAction_ == action) {return;}
    if (currentAction_ == DisplayerAction::Zoom) {
        zoomButton->setChecked(false);
    }
    currentAction_ = action;
}

void MainWindow::zoomSlot()
{
    if (not mainStackedWidget->count()) {return;}
    setCurrentAction(DisplayerAction::Zoom);
}

void MainWindow::resetZoomSlot()
{
    if (not mainStackedWidget->count()) {return;}
    qobject_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget())->resetZoom();
}





