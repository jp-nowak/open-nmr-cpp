#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/tabwidget.h"

#include <QMainWindow>

#include <tuple>

class Spectrum;

class TabWidget;
class PhaseCorrectionWidget;
class ZeroFillingWidget;

enum class DisplayerAction;

class QPushButton;
class QStackedWidget;
class QFrame;
class QDockWidget;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

    friend class PhaseCorrectionWidget;

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setCurrentAction(DisplayerAction action);

signals:

    void displayedSpectrumChanged(Spectrum* experiment);
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

    QAction* openFileAction;
    QAction* closeAppAction;
    QAction* phaseCorrectionAction;
    QAction* zeroFillingAction;

    QStackedWidget* mainStackedWidget;

    QPushButton* openFileButton;
    QPushButton* zoomButton;
    QPushButton* zoomResetButton;
    QPushButton* integrateButton;
    QPushButton* resetIntegralsButton;

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

};

#endif // MAINWINDOW_H
