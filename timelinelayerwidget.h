#ifndef TIMELINELAYERWIDGET_H
#define TIMELINELAYERWIDGET_H

#include <QWidget>
#include <memory>

namespace Ui {
class TimelineLayerWidget;
}

class TimelineLayer;
class TimelineEventsWidget;
class QLabel;

class TimelineLayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineLayerWidget(const std::shared_ptr<TimelineLayer>& layer, QWidget *parent = 0);
    ~TimelineLayerWidget();

public slots:
    void setViewport(double startTime, double length);
    void setCursor(double time);

private:
    Ui::TimelineLayerWidget *ui;

    QLabel* mLblLayerName;
    TimelineEventsWidget* mEventsWidget;

    std::shared_ptr<TimelineLayer> mLayer;
};

#endif // TIMELINELAYERWIDGET_H
