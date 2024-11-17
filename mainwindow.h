#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenuBar;
class Qmenu;
class QAction;


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


    QMenuBar* topMenuBar;
    QMenu* fileMenu;


    QAction* openFileAction;
    QAction* closeAppAction;
private slots:
    void openFileSlot();


};
#endif // MAINWINDOW_H
