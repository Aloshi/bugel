#include "timeline.h"

#include <QDebug>

Timeline::Timeline()
{

}

void Timeline::createLayer()
{
    QString name = QString("layer_%1").arg(mLayers.size());
    mLayers.push_back(std::make_shared<TimelineLayer>(name));
    emit layerAdded(mLayers.size() - 1, mLayers.last());
}



TimelineLayer::TimelineLayer(const QString& name)
    : mName(name)
{
}

void TimelineLayer::setScript(const QString& name)
{
    mScript = name;
    emit scriptChanged(mScript);
}


void EventList::addEvent(const std::shared_ptr<TimelineEvent>& event)
{
    mEvents.push_back(event);
    std::sort(mEvents.begin(), mEvents.end(),
              [](const std::shared_ptr<TimelineEvent>& e1, const std::shared_ptr<TimelineEvent>& e2) -> bool {
        return e1->time() < e2->time();
    });
}

void EventList::removeEvent(const std::shared_ptr<TimelineEvent>& event)
{
    mEvents.removeOne(event);
}

void EventList::removeEventsInRange(double start, double end)
{
    auto events = eventsInRange(start, end);
    for (auto it = events.begin(); it != events.end(); it++) {
        removeEvent(*it);
    }
}

QVector< std::shared_ptr<TimelineEvent> > EventList::eventsInRange(double start, double end) const
{
    QVector< std::shared_ptr<TimelineEvent> > ret;
    for (int i = 0; i < mEvents.size(); i++) {
        if (mEvents[i]->time() >= end)
            break;
        if (mEvents[i]->time() >= start)
            ret.push_back(mEvents[i]);
    }
    return ret;
}
