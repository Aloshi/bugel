#ifndef NOTEEVENT_H
#define NOTEEVENT_H

#include "timeline.h"

class NoteEvent : public TimelineEvent
{
public:
    NoteEvent() : TimelineEvent(), mNote(-1) {}
    NoteEvent(double time, int note) : TimelineEvent(time), mNote(note) {}
    virtual ~NoteEvent() {}

    int editorRow() const /* override */;
    QColor editorColor() const /* override */;

private:
    int mNote;
};

#endif // NOTEEVENT_H
