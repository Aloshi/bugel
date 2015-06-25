#include "timelinelayerwidget.h"
#include "ui_timelinelayerwidget.h"

#include <QLabel>

#include "timeline.h"
#include "timelineeventswidget.h"

TimelineLayerWidget::TimelineLayerWidget(const std::shared_ptr<TimelineLayer>& layer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimelineLayerWidget),
    mLayer(layer)
{
    ui->setupUi(this);

    mLblLayerName = findChild<QLabel*>("layer_name");
    mEventsWidget = findChild<TimelineEventsWidget*>("events");

    mLblLayerName->setText(mLayer->name());
    mEventsWidget->setEventSource(mLayer.get());
}

TimelineLayerWidget::~TimelineLayerWidget()
{
    delete ui;
}

void TimelineLayerWidget::setViewport(double startTime, double length)
{
    mEventsWidget->setViewport(startTime, length);
}

void TimelineLayerWidget::setCursor(double time)
{
    mEventsWidget->setCursor(time);
}
