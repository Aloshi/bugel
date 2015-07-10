#ifndef TIMELINECONTAINER_H
#define TIMELINECONTAINER_H

#include <QWidget>

#include "timeline.h"
#include "selection.h"

namespace Ui {
class TimelineContainer;
}

class QScrollArea;
class QVBoxLayout;
class TimelineWidget;
class PlaybackWidget;
class QMediaContent;

// controller and view container
class TimelineContainer : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineContainer(QWidget *parent = 0);
    ~TimelineContainer();

    double cursor() const;
    inline std::shared_ptr<Timeline> timeline() { return mTimeline; }

    void setTimeline(const std::shared_ptr<Timeline>& timeline);

protected:
    void wheelEvent(QWheelEvent* ev) /* override */;

signals:
    void currentLayerChanged(int idx); // idx == -1 if no layer is selected.
    void currentSelectionChanged(const Selection& selection);

public slots:
    void createLayer();
    void removeCurrentLayer();
    void addEventToCurrentLayer(const std::shared_ptr<TimelineEvent>& event);
    void removeSelectionInCurrentLayer();

private slots:
    void insertLayerWidget(int idx, std::shared_ptr<TimelineLayer> layer);
    void removeLayerWidget(int idx);

private:
    int currentLayerIdx() const;
    const Selection& currentSelection() const;
    std::shared_ptr<TimelineLayer> currentLayer();
    void setCurrentLayerIdx(int idx);

    Ui::TimelineContainer *ui;

    QScrollArea* mLayerScrollArea;
    QVBoxLayout* mLayerLayout;
    PlaybackWidget* mPlaybackWidget;
    TimelineWidget* mTimelineWidget;

    int mCurrentLayerIdx;
    std::shared_ptr<Timeline> mTimeline;
};

#endif // TIMELINECONTAINER_H
