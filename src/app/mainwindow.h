#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QFrame>
#include <QPen>
#include <QList>
#include <QStackedWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:


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


private slots:
    void openFileSlot();
    void zoomSlot();
    void resetZoomSlot();
};

#endif // MAINWINDOW_H
