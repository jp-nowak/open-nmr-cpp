#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/spectrum_displayer_actions.h"
#include "gui/tabwidget.h"

#include <QMainWindow>

class QPushButton;
class QStackedWidget;
class QFrame;
class QDockWidget;

class TabWidget;
class PhaseCorrectionWidget;


class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class PhaseCorrectionWidget;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setCurrentAction(DisplayerAction action);

private:

    DisplayerAction currentAction_;


    // helper functions for constructor
    void createActions();
    void createTopMenuBar();
    void createActionsFrame();

    QFrame* actionsFrame;
    QMenuBar* topMenuBar;

    QAction* openFileAction;
    QAction* closeAppAction;
    QAction* phaseCorrectionAction;

    QStackedWidget* mainStackedWidget;

    QPushButton* openFileButton;
    QPushButton* zoomButton;
    QPushButton* zoomResetButton;
    QPushButton* integrateButton;

    TabWidget* tabWidget;

    // processing widgets

    PhaseCorrectionWidget* phaseCorrectionWidget;
    QDockWidget* phaseCorrectionWidgetDock;




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
    void spectrumChangedSlot(int i);
    void phaseCorrectionSlot();
};

#endif // MAINWINDOW_H
