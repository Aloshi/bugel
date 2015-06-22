#ifndef TIMELINECONTAINER_H
#define TIMELINECONTAINER_H

#include <QWidget>

#include "timeline.h"

namespace Ui {
class TimelineContainer;
}

class QScrollArea;
class QVBoxLayout;
class TimelineWidget;
class PlaybackWidget;

class TimelineContainer : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineContainer(QWidget *parent = 0);
    ~TimelineContainer();

public slots:
    void createLayer();

private slots:
    void insertLayerWidget(int idx, std::shared_ptr<TimelineLayer> layer);
    void removeLayerWidget(int idx);

private:
    Ui::TimelineContainer *ui;

    QScrollArea* mLayerScrollArea;
    QVBoxLayout* mLayerLayout;
    PlaybackWidget* mPlaybackWidget;
    TimelineWidget* mTimelineWidget;

    Timeline mTimeline;
};

#endif // TIMELINECONTAINER_H
