#include "mainwindow.h"

#include "file_io/general.h"
#include "processing/general.h"
#include "spectrum/spectrum.h"
#include "gui/spectrumdisplayer.h"
#include "gui/spectrumpainter.h"

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
    , mainStackedWidget(new QStackedWidget())
{
    resize(screen()->availableGeometry().size() * 0.7);
    #ifdef DEBUG__
        qDebug() << "debug";
    #endif

    setWindowTitle(tr("Open NMR"));
    createActions();
    createTopMenuBar();
    createActionsFrame();

    QWidget* mainWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout();

    QVBoxLayout* rightSideLayout = new QVBoxLayout();

    rightSideLayout->addWidget(actionsFrame);
    rightSideLayout->addWidget(new QLabel(tr("TODO"), this));

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

    QPushButton* openFileButton = new QPushButton(tr("Open File"), actionsFrame);
    connect(openFileButton, &QPushButton::clicked, this, &MainWindow::openFileSlot);

    QPushButton* zoomButton = new QPushButton(tr("Zoom"), actionsFrame);
    connect(zoomButton, &QPushButton::clicked, this, &MainWindow::zoomSlot);

    QPushButton* zoomResetButton = new QPushButton(tr("Reset Zoom"), actionsFrame);
    connect(zoomResetButton, &QPushButton::clicked, this, &MainWindow::resetZoomSlot);

    QPushButton* integrateButton = new QPushButton(tr("Integrate"), actionsFrame);

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
    qDebug() << selectedFile << "\n";
    qDebug() << selectedFile.toStdString() << "\n";
    std::filesystem::path input_path{selectedFile.toStdString()};
    FileIO::FileReadResult file_read_result = FileIO::open_experiment(input_path);

    std::unique_ptr<Spectrum> experiment = Spectrum::pointer_from_file_read_result(file_read_result);

    SpectrumDisplayer* spectrumDisplayer = new SpectrumDisplayer(std::move(experiment));


    qDebug() << "painter created";
    mainStackedWidget->addWidget(spectrumDisplayer);
    #ifdef DEBUG__
    qDebug() << experiment.info;
    #endif


    qDebug() << file_read_result.info.obs_nucleus_freq;
    qDebug() << "koniec";


}

void MainWindow::zoomSlot()
{
    if (not mainStackedWidget->count()) {return;}
    qobject_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget())->currentAction = DisplayerAction::Zoom;
}

void MainWindow::resetZoomSlot()
{
    if (not mainStackedWidget->count()) {return;}
    qobject_cast<SpectrumDisplayer*>(mainStackedWidget->currentWidget())->resetZoom();
}
