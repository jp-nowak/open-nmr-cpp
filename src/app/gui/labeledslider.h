#ifndef LABELEDSLIDER_H
#define LABELEDSLIDER_H


#include <optional>
#include <QObject>
#include <QWidget>

class QVBoxLayout;
class QSlider;
class XAxis;

class LabeledSlider : public QWidget
{
    Q_OBJECT

public:

    LabeledSlider(double minimum, double maximum,
                  double interval, unsigned int precision,
                  QWidget* parent);

    // void paintEvent(QPaintEvent* e) override;

private:
    QVBoxLayout* layout;
    XAxis* labels;
    QSlider* slider;

};

#endif // LABELEDSLIDER_H
