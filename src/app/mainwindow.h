#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/spectrum_displayer_actions.h"
#include "gui/tabwidget.h"

#include <QMainWindow>

class QPushButton;
class TabWidget;
class QStackedWidget;
class QFrame;

class MainWindow : public QMainWindow
{
    Q_OBJECT

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
    QMenu* fileMenu;

    QAction* openFileAction;
    QAction* closeAppAction;

    QStackedWidget* mainStackedWidget;

    QPushButton* openFileButton;
    QPushButton* zoomButton;
    QPushButton* zoomResetButton;
    QPushButton* integrateButton;

    TabWidget* tabWidget;



 public:
    DisplayerAction const& currentAction;

    static MainWindow* findFrom(QWidget* widget) {
        while (widget) {
            MainWindow* foo = qobject_cast<MainWindow*>(widget);
            if (foo) {
                return foo; // found a Foo parent
            }
            widget = widget->parentWidget();
        }
        return nullptr; // no Foo parent found
    }




private slots:
    void openFileSlot();
    void zoomSlot();
    void resetZoomSlot();
};

#endif // MAINWINDOW_H
