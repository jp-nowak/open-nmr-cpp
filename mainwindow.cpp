#include "mainwindow.h"

#include "file_io/general.h"
#include "processing/general.h"
#include "spectrum/spectrum.h"

#include <filesystem>

#include <QMenuBar>
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

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Open NMR"));
    createActions();
    createTopMenuBar();
}

MainWindow::~MainWindow() {}

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
    if (fileDialog.selectedFiles().isEmpty()) {
        return;
    }
    QString selectedFile = fileDialog.selectedFiles().at(0);
    qDebug() << selectedFile << "\n";
    qDebug() << selectedFile.toStdString() << "\n";
    std::filesystem::path input_path{selectedFile.toStdString()};
    FileIO::FileReadResult file_read_result = FileIO::open_experiment(input_path);

    std::vector<std::complex<double>> spectrum = Processing::generate_spectrum_from_fid(file_read_result.fids[0]);
    Spectrum experiment{file_read_result.info, file_read_result.fids[0], spectrum};

    qDebug() << file_read_result.info.obs_nucleus_freq;
    qDebug() << "koniec";
}
