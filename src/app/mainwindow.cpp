#include "mainwindow.h"
#include "ui_mainwindow.h"

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
#include <QPen>
#include <QList>

SpectrumPainter::SpectrumPainter(QWidget *parent)
    : QWidget(parent), drawstatus(false), textfont("Times New Roman", 10), pen(QColor("black")), width_vis{0, 1}
{

    // Initializing properties
    initializeProperties();
    qDebug() << "SpectrumPainter object created:" << this;
}

void SpectrumPainter::setExperiment(const Spectrum &experiment)
{
}

void SpectrumPainter::initializeProperties()
{
    // Actions status and info initialization
    current_action = "";
    sel_region = {{0, 0}, {0, 0}};
    presspos = QPoint();
    currentpos = QPoint();
    actions_range = {"integ_man", "zooming", "peak_man"};
    actions_box = {"removing"};
    actions_pick_element = {"signal_auto"};
}

ActionsWidget::ActionsWidget(QWidget *parent) : QFrame(parent)
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

    // Connections
    connect(fileButton, &QPushButton::clicked, this, &ActionsWidget::openFileClicked);

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
{
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainWindow)
{
    QList<QColor> additional_palette = {
        QColor("#558B6E"), // accent
        QColor("#3B614D"), // accent-dark
        QColor("#C0C0C0")  // gray
    };

    ui->setupUi(this);
    actionsFrame = new ActionsWidget(this);
    spectrumStack = new QStackedWidget();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(spectrumStack);
    mainLayout->addWidget(actionsFrame);

    setLayout(mainLayout);
    setWindowTitle(tr("Open NMR"));
    createActions();
    createTopMenuBar();

    // actions
    connect(actionsFrame, &ActionsWidget::openFileClicked, this, &MainWindow::openFileSlot);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    topMenuBar = new QMenuBar(this);

    fileMenu = topMenuBar->addMenu(tr("File"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(closeAppAction);
}

void MainWindow::addToStack(const Spectrum &experiment)
{
    SpectrumPainter *painterwidget = new SpectrumPainter();
    painterwidget->setExperiment(experiment); // Add a method to handle this in SpectrumPainter if necessary
    spectrumStack->addWidget(painterwidget);  // Correctly use spectrumStack
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

    std::vector<std::complex<double>> spectrum = Processing::generate_spectrum_from_fid(file_read_result.fids[0]);
    Spectrum experiment{file_read_result.info, file_read_result.fids[0], spectrum};

    qDebug() << file_read_result.info.obs_nucleus_freq;
    qDebug() << "koniec";

    addToStack(experiment);
}
