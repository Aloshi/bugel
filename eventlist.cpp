#include "eventlist.h"

#include "timelineevent.h"

void EventList::addEvent(const std::shared_ptr<TimelineEvent>& event)
{
    Q_ASSERT(event != nullptr);
    mEvents.push_back(event);
    std::sort(mEvents.begin(), mEvents.end(),
              [](const std::shared_ptr<TimelineEvent>& e1, const std::shared_ptr<TimelineEvent>& e2) -> bool {
        return e1->time() < e2->time();
    });
    emit eventAdded(event);
}

void EventList::removeEvent(const std::shared_ptr<TimelineEvent>& event)
{
    mEvents.removeOne(event);
    emit eventRemoved(event);
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

QJsonArray EventList::toJSON() const
{
    QJsonArray events;
    for (auto it = mEvents.begin(); it != mEvents.end(); it++) {
        events.append(it->get()->toJSON());
    }
    return events;
}

void EventList::fromJSON(const QJsonArray& events)
{
    for (auto it = events.begin(); it != events.end(); it++) {
        addEvent(TimelineEvent::createFromJSON((*it).toObject()));
    }
}
