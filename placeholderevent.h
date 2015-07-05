#ifndef PLACEHOLDEREVENT_H
#define PLACEHOLDEREVENT_H

#include "timelineevent.h"

class PlaceholderEvent : public TimelineEvent
{
public:
    PlaceholderEvent(double time, int note)
        : TimelineEvent(PlaceholderEventType, time)
    {
        setProperty("note", note);
    }

private:
    static std::shared_ptr<EventType> PlaceholderEventType;
};

#endif // PLACEHOLDEREVENT_H
