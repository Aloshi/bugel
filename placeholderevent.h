#ifndef PLACEHOLDEREVENT_H
#define PLACEHOLDEREVENT_H

#include "timeline.h"

class PlaceholderEvent : public TimelineEvent
{
public:
    PlaceholderEvent() : TimelineEvent(), mNote(-1) {}
    PlaceholderEvent(double time, int note) : TimelineEvent(time), mNote(note) {}
    virtual ~PlaceholderEvent() {}

    const char* type() const /* override */;
    int editorRow() const /* override */;
    QColor editorColor() const /* override */;

    void writeParametersJSON(QJsonObject& ev) const /* override */;
    void readParametersJSON(const QJsonObject& ev) /* override */;

private:
    int mNote;
};

#endif // PLACEHOLDEREVENT_H
