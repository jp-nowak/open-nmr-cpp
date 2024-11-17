#include "mainwindow.h"

#include "file_io/general.h"
#include "processing/general.h"
#include "spectrum/spectrum.h"

#include <filesystem>

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

ActionsWidget::ActionsWidget(QFrame *parent) :
    QFrame(parent)
{
   // Viewing buttons
   QPushButton *fileButton = new QPushButton(tr("Open File"), this);
   QPushButton *zoomButton = new QPushButton(tr("Zoom"), this);
   QPushButton *zoomResetButton = new QPushButton(tr("Reset Zoom"), this);

   // Integration buttons
   QPushButton *manualIntegButton = new QPushButton(tr("Manual Integration"), this);
   QPushButton *removeButton = new QPushButton(tr("Remove Integral"), this);
   QPushButton *resetIntegralsButton = new QPushButton(tr("Reset Integrals"), this);

   // Other buttons
   QPushButton *manualPeakButton = new QPushButton(tr("Manual Peaks"), this);
   QPushButton *autoPeakButton = new QPushButton(tr("Auto Signal"), this);

   // Set checkable properties
   zoomButton->setCheckable(true);
   manualIntegButton->setCheckable(true);
   removeButton->setCheckable(true);
   manualPeakButton->setCheckable(true);
   autoPeakButton->setCheckable(true);

   // Layout setup
   QVBoxLayout *actionsLayout = new QVBoxLayout;
   actionsLayout->addWidget(new QLabel(tr("Viewing")));
   actionsLayout->addWidget(fileButton);
   actionsLayout->addWidget(zoomButton);
   actionsLayout->addWidget(zoomResetButton);

   actionsLayout->addWidget(new QLabel(tr("Integration")));
   actionsLayout->addWidget(manualIntegButton);
   actionsLayout->addWidget(removeButton);
   actionsLayout->addWidget(resetIntegralsButton);

   actionsLayout->addWidget(new QLabel(tr("Other")));
   actionsLayout->addWidget(manualPeakButton);
   actionsLayout->addWidget(autoPeakButton);
   actionsLayout->setAlignment(Qt::AlignTop);

   // Set layout
   setLayout(actionsLayout);
}

// Destructor
ActionsWidget::~ActionsWidget()
{}

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    actionsFrame = new ActionsWidget();
    spectrumStack = new QStackedWidget();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(spectrumStack);
    mainLayout->addWidget(actionsFrame);

    setLayout(mainLayout);
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
    topMenuBar = new QMenuBar(this);

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
