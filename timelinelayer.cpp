#include "timelinelayer.h"

TimelineLayer::TimelineLayer(const QString& name)
    : mName(name)
{
}

void TimelineLayer::setName(const QString& name)
{
    mName = name;
    emit nameChanged(mName);
}

void TimelineLayer::setScript(const QString& name)
{
    mScript = name;
    emit scriptChanged(mScript);
}

QJsonObject TimelineLayer::toJSON() const
{
    QJsonObject layer;
    layer["name"] = name();
    layer["script"] = script();
    layer["events"] = mEvents.toJSON();
    return layer;
}

void TimelineLayer::fromJSON(const QJsonObject &layer)
{
    setName(layer["name"].toString());
    setScript(layer["script"].toString());
    mEvents.fromJSON(layer["events"].toArray());
}
