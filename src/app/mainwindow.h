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

class SpectrumPainter : public QWidget {
    Q_OBJECT

public:
    SpectrumPainter(const QColor& palette2, QWidget* parent = nullptr);

    // there should be buttons here or sth

    // Member variables
    bool drawstatus;
    QFont textfont;
    QPen pen;
    QColor palette2;
    std::vector<int> width_vis;

    // Actions status and info
    QString current_action;
    std::vector<std::vector<int>> sel_region;
    QPoint presspos;
    QPoint currentpos;
    std::vector<QString> actions_range;
    std::vector<QString> actions_box;
    std::vector<QString> actions_pick_element;

protected:
    // Add your custom paintEvent or other event handlers here if needed
    // For example, paintEvent(QPaintEvent *event) override;

private:
    void initializeProperties();
};

namespace Ui {
class MainWindow;
}


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
    Ui::MainWindow* ui;

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
