#ifndef PROJECT_H
#define PROJECT_H

#include <QVector>
#include <memory>
#include <QJsonObject>

class Timeline;
struct EventType;

typedef QVector< std::shared_ptr<EventType> > EventTypeVector;
typedef QVector< std::shared_ptr<Timeline> > TimelineVector;
class Project
{
public:
    static Project* get();

    Project();

    std::shared_ptr<EventType> eventType(const QString& name);
    inline const EventTypeVector& eventTypes() const { return mEventTypes; }
    inline const TimelineVector& timelines() const { return mTimelines; }

    void setEventTypes(const EventTypeVector& eventTypes);

    std::shared_ptr<Timeline> createTimeline();
    void removeTimeline(const std::shared_ptr<Timeline>& timeline);

    void save(const QString& path) const;
    void load(const QString& path);

private:
    QJsonObject toJSON() const;
    void fromJSON(const QJsonObject& project);

    TimelineVector mTimelines;
    EventTypeVector mEventTypes;
};

#endif // PROJECT_H
