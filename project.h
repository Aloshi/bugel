#ifndef PROJECT_H
#define PROJECT_H

#include <QVector>
#include <memory>

struct EventType;

typedef QVector< std::shared_ptr<EventType> > EventTypeVector;
class Project
{
public:
    static Project* get();

    Project();

    inline const EventTypeVector& eventTypes() const { return mEventTypes; }

    void setEventTypes(const EventTypeVector& eventTypes);

private:
    QVector< std::shared_ptr<EventType> > mEventTypes;
};

#endif // PROJECT_H
