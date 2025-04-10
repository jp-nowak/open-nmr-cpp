#include "integralsdisplayer.h"

#include "../spectrum/spectrum.h"
#include "gui_utilities.h"


#include <QPainter>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QFontMetrics>

#include <utility>
#include <cmath>

IntegralsDisplayer::IntegralsDisplayer(const Spectrum* experiment, QWidget *parent)
    : QWidget{parent}
    , experiment{experiment}
    , integralEditField{nullptr}
    , editedIntegral{nullptr}
    , startPoint_{0}
    , endPoint_{experiment->get_spectrum().size()}
    , currentSpectrumSize{endPoint_}
    , pen{}
{
    pen.setCosmetic(true);
    pen.setWidth(2);
    pen.setColor(QColor(0, 255, 0));
    pen.setCapStyle(Qt::RoundCap);
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

    delete integralEditField; integralEditField = nullptr;
    editedIntegral = nullptr;

    update();

    QPointF position = e->pos();

    const double startPoint = startPoint_; // TODO create uniform struct with setting for IntegralsDisplayer and XAxis
    const double endPoint = endPoint_;
    const double width = this->width();

    auto xPosToDataPoint = [startPoint, endPoint, width](double x) -> size_t {
        return static_cast<size_t>(x * (endPoint - startPoint) / width + startPoint);
    };

    size_t dataPoint = xPosToDataPoint(position.x());

    qDebug() << __FUNCTION__;

    // choosing integrals that contain double clicked point
    std::vector<IntegralRecord*> fittingIntegrals{};
    for (auto& i : experiment->integrals) {
        if ((i.leftEdge < dataPoint) and (i.rightEdge > dataPoint)) {
            fittingIntegrals.push_back(&i);
        }
    } // TODO separate into method

    if (fittingIntegrals.empty()) return;

    // choosing closest integral to clicked point
    IntegralRecord* bestFit = fittingIntegrals[0];
    for (const auto i : fittingIntegrals) {
        size_t middlePoint = i->leftEdge + (i->rightEdge - i->leftEdge) / 2;
        size_t previousMiddlePoint = bestFit->leftEdge + (bestFit->rightEdge - bestFit->leftEdge) / 2;
        if ((std::abs(static_cast<long long>(dataPoint) - static_cast<long long>(middlePoint)))
            < (std::abs(static_cast<long long>(dataPoint) - static_cast<long long>(previousMiddlePoint)))) {
            bestFit = i;
            }
    }

    qDebug() << "best fit" << bestFit->relativeValue;

    editedIntegral = bestFit;

    update();


}


void IntegralsDisplayer::paintEvent(QPaintEvent* e)
{
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
    };


    for (const auto& i : experiment->integrals) {

        painter.setPen(pen);

        if ((i.rightEdge <= startPoint_) or (i.rightEdge > endPoint_)) continue;

        { // TODO to be separated into function
        QPolygonF line;
        line << QPointF(xPos(i.leftEdge), height * (1 - baselineHeight))
             << QPointF(xPos(i.rightEdge), height * (1 - baselineHeight));
        painter.drawPolyline(line);
        }

        {
        QPolygonF line;
        line << QPointF(xPos(i.leftEdge), height * (1 - baselineHeight - tickHeight))
             << QPointF(xPos(i.leftEdge), height * (1 - baselineHeight + tickHeight));
        painter.drawPolyline(line);
        }

        {
        QPolygonF line;
        line << QPointF(xPos(i.rightEdge), height * (1 - baselineHeight - tickHeight))
             << QPointF(xPos(i.rightEdge), height * (1 - baselineHeight + tickHeight));
        painter.drawPolyline(line);
        }

        auto text = QString::number(i.relativeValue, 'f', displayPrecision);
        auto point = QPointF(xPos(i.leftEdge + (i.rightEdge - i.leftEdge) / 2), e->rect().height() * (1 - baselineHeight + tickHeight));

        painter.setPen(QColor("black"));

        drawText(painter,
                 point,
                 Qt::AlignHCenter | Qt::AlignTop,
                 text);
    }

    if (editedIntegral and not integralEditField) {
        integralEditField = new QDoubleSpinBox{this};
        integralEditField->setButtonSymbols(QAbstractSpinBox::NoButtons);

        auto xPos = [startPoint, endPoint, width](double x) -> double{
            return (x - startPoint) / (endPoint - startPoint) * width;
        };

        auto text = QString::number(editedIntegral->relativeValue, 'f', displayPrecision);

        QFontMetrics fm = painter.fontMetrics();
        int textWidth = fm.boundingRect(text).width();


        auto point = QPoint(xPos(editedIntegral->leftEdge
                            + (editedIntegral->rightEdge - editedIntegral->leftEdge) / 2)
                            - textWidth / 2,
                            height * (1 - baselineHeight + tickHeight));

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
           delete integralEditField;
           integralEditField = nullptr;
           editedIntegral = nullptr;
           update();
        });

        integralEditField->show();
    }


}
