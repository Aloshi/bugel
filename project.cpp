#include "project.h"

#include "timelineevent.h"

Project* Project::get()
{
    static Project* gProject = NULL;
    if (!gProject) {
        gProject = new Project();
    }
    return gProject;
}

Project::Project()
{
    auto placeholderType = std::make_shared<EventType>("placeholder");
    placeholderType->properties["note"] = EventProperty(QMetaType::Int, QVariant(0));

    mEventTypes.push_back(placeholderType);
}

void Project::setEventTypes(const EventTypeVector& eventTypes)
{
    mEventTypes = eventTypes;

    // TODO: update timeline
}
