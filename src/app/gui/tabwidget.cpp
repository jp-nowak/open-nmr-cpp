#include "tabwidget.h"

#include "spectrumdisplayer.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QStyleOptionButton>


TabWidget::TabWidget(QStackedWidget* stackedSpectra, QWidget* parent)
    : QWidget{parent}
    , stackedSpectra{stackedSpectra}
{
    layout = new QVBoxLayout(this);
    setLayout(layout);
    layout->addWidget(new QLabel(tr("Open spectra"), this));
    grid = new QGridLayout(this);
    layout->addLayout(grid);
    layout->setAlignment(Qt::AlignTop);
    grid->setAlignment(Qt::AlignTop);
    grid->setHorizontalSpacing(0);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(1, 0);
}

void TabWidget::addTab(SpectrumDisplayer* newSpectrum)
{
    QPushButton* newButton = new QPushButton(QString::fromStdString(newSpectrum->experiment->info.samplename), this);

    connect(newButton, &QPushButton::clicked, this,
            [newSpectrum, this]()
            {
                this->stackedSpectra->setCurrentWidget(newSpectrum);
            }
    );

    QPushButton* delButton = new QPushButton(QString("x"), this);

    // sets minimal size of button to size of text in it
    auto textSize = delButton->fontMetrics().size(Qt::TextShowMnemonic, delButton->text());
    QStyleOptionButton opt;
    opt.initFrom(delButton);
    opt.rect.setSize(textSize);
    delButton->setMinimumSize(
        delButton->style()->sizeFromContents(QStyle::CT_PushButton,
                                          &opt,
                                          textSize,
                                          delButton));




    connect(delButton, &QPushButton::clicked, this,
            [newSpectrum, newButton, delButton, this]()
            {
                this->stackedSpectra->setCurrentIndex(this->stackedSpectra->currentIndex() - 1);
                this->stackedSpectra->removeWidget(newSpectrum);
                delete newSpectrum;
                delete newButton;
                delete delButton;
            }
            );
    size_t n = grid->rowCount();
    grid->addWidget(newButton, n, 0);
    grid->addWidget(delButton, n, 1);
}
