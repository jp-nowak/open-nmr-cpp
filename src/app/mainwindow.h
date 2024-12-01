#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QFrame>
#include <QPen>
#include <QList>
#include <QStackedWidget>

#include "spectrum/spectrum.h"
#include "gui/spectrumpainter.h"

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

    QMenuBar *topMenuBar;
    QMenu *fileMenu;

    QAction *openFileAction;
    QAction *closeAppAction;

    QStackedWidget* mainStackedWidget;


private slots:
    void openFileSlot();
};

#endif // MAINWINDOW_H
