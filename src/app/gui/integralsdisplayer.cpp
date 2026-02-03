#include "integralsdisplayer.h"

#include "../spectrum/spectrum.h"
#include "gui_utilities.h"
#include "../mainwindow.h"

#include <QPainter>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QFontMetrics>

#include <utility>
#include <cmath>

// TODO fix: when zooming into integral label, it is not displayed

namespace
{
//! finds integral from integrals which middle point was closest to xCoord
//! if clicled on point didnt overlap with any integral returns nullptr
IntegralRecord* findClickedOnIntegral(size_t xCoord, IntegralsVector& integrals)
{
    std::vector<IntegralRecord*> fittingIntegrals{};
    for (auto& i : integrals) {
        if ((i.leftEdge < xCoord) and (i.rightEdge > xCoord)) {
            fittingIntegrals.push_back(&i);
        }
    } // TODO separate into method

    if (fittingIntegrals.empty()) return nullptr;

    // choosing closest integral to clicked point
    IntegralRecord* bestFit = fittingIntegrals[0];
    for (const auto i : fittingIntegrals) {
        size_t middlePoint = i->leftEdge + (i->rightEdge - i->leftEdge) / 2;
        size_t previousMiddlePoint = bestFit->leftEdge + (bestFit->rightEdge - bestFit->leftEdge) / 2;
        if ((std::abs(static_cast<long long>(xCoord) - static_cast<long long>(middlePoint)))
            < (std::abs(static_cast<long long>(xCoord) - static_cast<long long>(previousMiddlePoint)))) {
            bestFit = i;
            }
    }
    return bestFit;
}
}

IntegralsDisplayer::IntegralsDisplayer(const Spectrum_1D* experiment, QWidget *parent)
    : QWidget{parent}
    , integralEditField{nullptr}
    , experiment{experiment}
    , editedIntegral{nullptr}
    , startPoint_{0}
    , endPoint_{experiment->get_spectrum().size()}
    , currentSpectrumSize{endPoint_}
    , pen{}
{
    setMouseTracking(true);
    pen.setCosmetic(true);
    pen.setWidth(2);
    pen.setColor(QColor(21,71,52));
    pen.setCapStyle(Qt::RoundCap);

    connect(MainWindow::findFrom(parent), &MainWindow::closeDynamicElements,
            this, &IntegralsDisplayer::closeIntegralEditField);

    setStyleSheet("background-color: rgba(0,0,0,0)");
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

void IntegralsDisplayer::setRange(size_t begin, size_t end)
{
    if (end < begin) {
        std::swap(begin, end);
    }

    if (end > experiment->get_spectrum().size()) return;

    startPoint_ = begin;
    endPoint_ = end;
}

void IntegralsDisplayer::zoom(QPointF startPos, QPointF endPos)
{
    double start = mapFromGlobal(startPos).x();
    double end = mapFromGlobal(endPos).x();

    const size_t oldStart = startPoint_;
    const size_t oldEnd = endPoint_;

    if (start > end) {
        std::swap(start, end);
    }

    start = (start < 0) ? 0 : start;
    end = (end > width()) ? width() : end;

    const size_t size = endPoint_ - startPoint_;

    endPoint_ = startPoint_ + std::floor(end / width() * (size - 1));
    startPoint_ += std::ceil(start / width() * (size - 1));

    if (startPoint_ + 5 > endPoint_) {
        startPoint_ = oldStart;
        endPoint_ = oldEnd;
        return;
    } // stops user from zooming to close
    update();
}

void IntegralsDisplayer::resetZoom()
{
    startPoint_ = 0;
    endPoint_ = experiment->get_spectrum().size();
    update();
}


void IntegralsDisplayer::recalculateDisplayRange()
{
    assert(currentSpectrumSize != 0);
    if (size_t n = experiment->get_spectrum().size(); n == currentSpectrumSize) {
        return;
    } else {
        startPoint_ = static_cast<double>(startPoint_) / currentSpectrumSize * n;
        endPoint_ = static_cast<double>(endPoint_) / currentSpectrumSize * n;
        currentSpectrumSize = n;
    }
}

void IntegralsDisplayer::mouseDoubleClickEvent(QMouseEvent* e)
{

    QWidget::mouseDoubleClickEvent(e);

    delete integralEditField; integralEditField = nullptr;
    editedIntegral = nullptr;

    QPointF position = e->pos();

    const double startPoint = startPoint_; // TODO create uniform struct with setting for IntegralsDisplayer and XAxis
    const double endPoint = endPoint_;
    const double width = this->width();

    auto xPosToDataPoint = [startPoint, endPoint, width](double x) -> size_t {
        return static_cast<size_t>(x * (endPoint - startPoint) / width + startPoint);
    };

    size_t dataPoint = xPosToDataPoint(position.x());

    editedIntegral = findClickedOnIntegral(dataPoint, experiment->integrals);

    update();
}


void IntegralsDisplayer::paintEvent(QPaintEvent* e)
{
    QWidget::paintEvent(e);
    recalculateDisplayRange();

    QPainter painter{this};

    const double startPoint = startPoint_; // TODO create uniform struct with setting for IntegralsDisplayer and XAxis
    const double endPoint = endPoint_;
    const double width = e->rect().width();
    const double height = e->rect().height();
    constexpr double baselineHeight = 0.5;
    constexpr double displayPrecision = 2;
    constexpr double tickHeight = 0.1;

    auto xPos = [startPoint, endPoint, width](double x) -> double{
        return (x - startPoint) / (endPoint - startPoint) * width;
    }; // calculates position of spectrum datapoint as x coord of widget

    auto xPosToDataPoint = [startPoint, endPoint, width](double x) -> size_t {
        return static_cast<size_t>(x * (endPoint - startPoint) / width + startPoint);
    };

    // checking if mouse points on any integral in both horizontal and vertical dimension
    // TODO : check only if mouse is close to integral not if it is exactly pointing on it
    IntegralRecord* pointedIntegral = nullptr;
    if (std::fabs(mousePosition.y() - height * baselineHeight) < 0.2 * height) {
        size_t mouseDataPoint = xPosToDataPoint(mousePosition.x());
        pointedIntegral = findClickedOnIntegral(mouseDataPoint, experiment->integrals);
    }

    for (const auto& i : experiment->integrals) {

        painter.setPen(pen);

        if ((i.rightEdge <= startPoint_) or (i.leftEdge > endPoint_)) continue; // omits integrals not visible in current view range

        if (pointedIntegral != &i) { // integral range which is pointed on is drawed differently
            drawRangeWithMarks(painter,
                               QPointF(xPos(i.leftEdge), height * (1 - baselineHeight)),
                               QPointF(xPos(i.rightEdge), height * (1 - baselineHeight)),
                               tickHeight * height
                               );
        }

        auto text = QString::number(i.relativeValue, 'f', displayPrecision);

        auto point = QPointF(xPos(i.leftEdge + (i.rightEdge - i.leftEdge) / 2), e->rect().height() * (1 - baselineHeight + tickHeight));

        auto alignment = Qt::AlignHCenter;

        if (i.leftEdge < startPoint_) { // integrals stretching outside view range have labels aligned to internal side
            point.setX(xPos(i.rightEdge));
            alignment = Qt::AlignRight;
        } else if (i.rightEdge > endPoint_) {
            point.setX(xPos(i.leftEdge));
            alignment = Qt::AlignLeft;
        }

        painter.setPen(QColor("black"));

        if (editedIntegral == &i) continue; // label of currently edited integral is not drawn

         drawRotatedText90R(painter,
                point.x(), point.y() + 10,
                alignment | Qt::AlignTop,
                text);
    }

    if (pointedIntegral) { // integral pointed by mouse is highlighted
        painter.save();
        QPen pen{};
        pen.setCosmetic(true);
        pen.setWidth(4);
        pen.setColor(QColor(0, 255, 0, 100));
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);
        drawRangeWithMarks(painter,
                           QPointF(xPos(pointedIntegral->leftEdge), height * (1 - baselineHeight)),
                           QPointF(xPos(pointedIntegral->rightEdge), height * (1 - baselineHeight)),
                           tickHeight * height
        );
        painter.restore();
    }

    // mousePosition = {0, 0}; what was it for?

    if (editedIntegral and not integralEditField) { // block for showing integral edit field
        integralEditField = new QDoubleSpinBox{this};
        integralEditField->setButtonSymbols(QAbstractSpinBox::NoButtons);

        // auto xPos = [startPoint, endPoint, width](double x) -> double{
        //     return (x - startPoint) / (endPoint - startPoint) * width;
        // };

        auto text = QString::number(editedIntegral->relativeValue, 'f', displayPrecision);

        QFontMetrics fm = painter.fontMetrics();
        int textWidth = fm.boundingRect(text).width();

        auto point = QPoint(xPos(editedIntegral->leftEdge
                            + (editedIntegral->rightEdge - editedIntegral->leftEdge) / 2)
                            - textWidth / 2,
                            height * (1 - baselineHeight + tickHeight)); // tries to set point which makes edit field cover label of edited integral

        if (editedIntegral->leftEdge < startPoint_) {
            point.setX(xPos(editedIntegral->rightEdge) - textWidth);
        } else if (editedIntegral->rightEdge > endPoint_) {
            point.setX(xPos(editedIntegral->leftEdge) - textWidth / 2);
        }

        integralEditField->move(point);
        integralEditField->setValue(editedIntegral->relativeValue);

        connect(integralEditField, &QDoubleSpinBox::valueChanged, this, [this](double d){
            double valueOfOne = (d != 0.0) ? editedIntegral->absoluteValue / d : 0.0;
           recalcRelativeIntegralsValues(experiment->integrals, valueOfOne);
           update();
        });

        connect(integralEditField, &QDoubleSpinBox::editingFinished, this, [this](){
           double valueOfOne = (integralEditField->value() != 0.0) ? editedIntegral->absoluteValue / integralEditField->value() : 0.0;
           recalcRelativeIntegralsValues(experiment->integrals, valueOfOne);
           delete integralEditField; integralEditField = nullptr;
           editedIntegral = nullptr;
           update();
        });
        //integralEditField->setEnabled(true);
        //integralEditField->setWindowFlags(Qt::WindowStaysOnTopHint);
        integralEditField->selectAll();
        integralEditField->setFocus();
        integralEditField->show();
    }
    emit updated();
}

void IntegralsDisplayer::mousePressEvent(QMouseEvent* e)
{

    QWidget::mousePressEvent(e);
    closeIntegralEditField();
}

void IntegralsDisplayer::mouseMoveEvent(QMouseEvent* e)
{

    QWidget::mouseMoveEvent(e);
    mousePosition = e->pos();
    update();
}

// void IntegralsDisplayer::leaveEvent(QEvent* e)
// {
//     if (rect().contains(mapFromGlobal(QCursor::pos()))) return;

// }


void IntegralsDisplayer::closeIntegralEditField()
{
    delete integralEditField; integralEditField = nullptr;
    editedIntegral = nullptr;
    update();
}

