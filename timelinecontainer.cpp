#include "timelinecontainer.h"
#include "ui_timelinecontainer.h"

#include <QScrollArea>
#include <QVBoxLayout>

#include "timelinelayerwidget.h"
#include "timelinewidget.h"
#include "playbackwidget.h"

TimelineContainer::TimelineContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimelineContainer)
{
    ui->setupUi(this);

    mPlaybackWidget = findChild<PlaybackWidget*>("playback_widget");
    mTimelineWidget = findChild<TimelineWidget*>("timeline_widget");

    mLayerScrollArea = findChild<QScrollArea*>("scroll_layers");
    mLayerLayout = dynamic_cast<QVBoxLayout*>(mLayerScrollArea->widget()->layout());

    QObject::connect(&mTimeline, &Timeline::layerAdded,
                     this, &TimelineContainer::insertLayerWidget);
    QObject::connect(&mTimeline, &Timeline::layerRemoved,
                     this, &TimelineContainer::removeLayerWidget);

    QObject::connect(mPlaybackWidget, &PlaybackWidget::positionChanged,
                     mTimelineWidget, &TimelineWidget::setCursor);
    QObject::connect(mPlaybackWidget, &PlaybackWidget::durationChanged,
                     mTimelineWidget, &TimelineWidget::setLength);
    QObject::connect(mTimelineWidget, &TimelineWidget::timeClicked,
                     mPlaybackWidget, &PlaybackWidget::setPosition);

    mPlaybackWidget->setMedia(QUrl::fromLocalFile("/Users/Aloshi/Dropbox/Public/4.mp3"));
}

TimelineContainer::~TimelineContainer()
{
    delete ui;
}

void TimelineContainer::createLayer()
{
    mTimeline.createLayer();
}

void TimelineContainer::insertLayerWidget(int idx, std::shared_ptr<TimelineLayer> layer)
{
    TimelineLayerWidget* widget = new TimelineLayerWidget(layer);
    QObject::connect(mTimelineWidget, &TimelineWidget::cursorMoved,
                     widget, &TimelineLayerWidget::setCursor);
    QObject::connect(mTimelineWidget, &TimelineWidget::viewportChanged,
                     widget, &TimelineLayerWidget::setViewport);

    mLayerLayout->insertWidget(idx, widget);
}

void TimelineContainer::removeLayerWidget(int idx)
{
    mLayerLayout->removeWidget((QWidget*)mLayerLayout->children().at(idx));
}
