#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/spectrum_displayer_actions.h"

#include <QMainWindow>
#include <QWidget>
#include <QFrame>
#include <QPen>
#include <QList>
#include <QStackedWidget>

class QPushButton;


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
