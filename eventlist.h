#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <QObject>
#include <QVector>
#include <QJsonArray>
#include <memory>

class TimelineEvent;

class EventList : public QObject
{
    Q_OBJECT
public:
    void addEvent(const std::shared_ptr<TimelineEvent>& event);
    void removeEvent(const std::shared_ptr<TimelineEvent>& event);
    void removeEventsInRange(double start, double end);
    QVector< std::shared_ptr<TimelineEvent> > eventsInRange(double start, double end) const;

    inline QVector< std::shared_ptr<TimelineEvent> >::const_iterator begin() const {
        return mEvents.cbegin();
    }
    inline QVector< std::shared_ptr<TimelineEvent> >::const_iterator end() const {
        return mEvents.cend();
    }
    inline int size() const {
        return mEvents.size();
    }

    QJsonArray toJSON() const;
    void fromJSON(const QJsonArray& events);

signals:
    void eventAdded(const std::shared_ptr<TimelineEvent>& event);
    void eventRemoved(const std::shared_ptr<TimelineEvent>& event);

private:
    QVector< std::shared_ptr<TimelineEvent> > mEvents;
};

#endif // EVENTLIST_H
