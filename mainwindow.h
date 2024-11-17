#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QFrame>

class QMenuBar;
class Qmenu;
class QAction;
class QFrame;
class QStackedWidget;
class QPushButton;

class ActionsWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ActionsWidget(QFrame *parent = 0); //Constructor
    ~ActionsWidget(); // Destructor

private:
    QPushButton* fileButton;
    QPushButton* zoomButton;
    QPushButton* zoomResetButton;

    QPushButton* manualIntegButton;
    QPushButton* removeButton;
    QPushButton* resetIntegralsButton;

    QPushButton* manualPeakButton;
    QPushButton* autoPeakButton;
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ActionsWidget* actionsFrame;
    QStackedWidget* spectrumStack;

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
