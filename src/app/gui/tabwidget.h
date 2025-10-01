#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QObject>
#include <QWidget>

class QStackedWidget;
class QVBoxLayout;
class QGridLayout;

class SpectrumDisplayer_1D;

class TabWidget : public QWidget
{
    Q_OBJECT
public:
    TabWidget(QStackedWidget* stackedSpectra, QWidget* parent);

    void addTab(SpectrumDisplayer_1D* newSpectrum);



private:
    QStackedWidget* stackedSpectra;
    QVBoxLayout* layout;
    QGridLayout* grid;

signals:
};

#endif // TABWIDGET_H


