#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "file_io/general.h"
#include "processing/general.h"
#include "spectrum/spectrum.h"
#include "gui/spectrumdisplayer.h"
#include "gui/spectrumpainter.h"

#include <filesystem>

#include <vector>
#include <complex>
#include <algorithm>

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
    #ifdef DEBUG__
        qDebug() << "debug";
    #endif

    setWindowTitle(tr("Open NMR"));
    createActions();
    createTopMenuBar();

    setCentralWidget(mainStackedWidget);
    // actions .

}

MainWindow::~MainWindow()
{
    setGeometry(200, 200, 800, 400);
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
    SpectrumPainter* spectrumPainter = new SpectrumPainter{std::move(experiment)};
    SpectrumDisplayer* spectrumDisplayer = new SpectrumDisplayer(spectrumPainter);


    qDebug() << "painter created";
    mainStackedWidget->addWidget(spectrumDisplayer);
    #ifdef DEBUG__
    qDebug() << experiment.info;
    #endif


    qDebug() << file_read_result.info.obs_nucleus_freq;
    qDebug() << "koniec";


}
