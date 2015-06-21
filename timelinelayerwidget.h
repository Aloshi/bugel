#ifndef TIMELINELAYERWIDGET_H
#define TIMELINELAYERWIDGET_H

#include <QWidget>
#include <memory>

namespace Ui {
class TimelineLayerWidget;
}

class TimelineLayer;
class QLabel;

class TimelineLayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineLayerWidget(const std::shared_ptr<TimelineLayer>& layer, QWidget *parent = 0);
    ~TimelineLayerWidget();

private:
    Ui::TimelineLayerWidget *ui;

    QLabel* mLblLayerName;

    std::shared_ptr<TimelineLayer> mLayer;
};

#endif // TIMELINELAYERWIDGET_H
