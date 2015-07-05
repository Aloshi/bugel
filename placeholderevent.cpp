#include "placeholderevent.h"

std::shared_ptr<EventType> makePlaceholderEventType()
{
    std::shared_ptr<EventType> type = std::make_shared<EventType>();
    type->name = "placeholder";
    type->properties["note"] = EventProperty(QMetaType::Int, QVariant(-1));
    return type;
}

std::shared_ptr<EventType> PlaceholderEvent::PlaceholderEventType = makePlaceholderEventType();
