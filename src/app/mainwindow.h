#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/tabwidget.h"

#include <QMainWindow>

#include <tuple>
#include <array>

class Spectrum_1D;

class TabWidget;
class PhaseCorrectionWidget;
class ZeroFillingWidget;

enum class DisplayerAction;

class QPushButton;
class QStackedWidget;
class QFrame;
class QDockWidget;

enum ButtonNames{openFileB, zoomB, zoomResetB, integrateB, integralsResetB, ActiveButton, NumberOfButtons};
enum ActionNames{openFileA, closeAppA, phaseCorrectionA, zeroFillingA, NumberofActions};

class MainWindow final : public QMainWindow
{
    Q_OBJECT

    friend class PhaseCorrectionWidget;

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:

    void displayedSpectrumChanged(Spectrum_1D* experiment);
    void closeDynamicElements();

private:

    DisplayerAction currentAction_;


    // helper functions for constructor
    void createActions();
    void createTopMenuBar();
    void createActionsFrame();
    void createKeyShortcuts();

    QFrame* actionsFrame;
    QMenuBar* topMenuBar;

    // QAction* openFileAction;
    // QAction* closeAppAction;
    // QAction* phaseCorrectionAction;
    // QAction* zeroFillingAction;

    QStackedWidget* mainStackedWidget;


    std::array<QPushButton*, NumberOfButtons> buttons = {};
    std::array<QAction*, NumberofActions> actions = {};

    // QPushButton* openFileButton;
    // QPushButton* zoomButton;
    // QPushButton* zoomResetButton;
    // QPushButton* integrateButton;
    // QPushButton* integralsResetButton;

    TabWidget* tabWidget;

    // processing widgets

    std::tuple<
        std::tuple<PhaseCorrectionWidget*, QDockWidget*>,
        std::tuple<ZeroFillingWidget*, QDockWidget*>> processingWidgets;

    template<typename T>
    void showProcessingWidget();

 public:
    DisplayerAction const& currentAction;

    static MainWindow* findFrom(QWidget* widget) {
        while (widget) {
            MainWindow* foo = qobject_cast<MainWindow*>(widget);
            if (foo) {
                return foo;
            }
            widget = widget->parentWidget();
        }
        return nullptr;
    }

private slots:
    void openFileSlot();
    void zoomSlot();
    void resetZoomSlot();
    void integrateSlot();
    void resetIntegralsSlot();
    void spectrumChangedSlot(int i);
    void phaseCorrectionSlot();
    void zeroFillingSlot();

public slots:
    void refreshCurrentDisplayerSlot();
    void finishAction();

};

#endif // MAINWINDOW_H
