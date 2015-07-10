#include "project.h"

#include "timeline.h"
#include "timelineevent.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>

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

void Project::load(const QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    fromJSON(doc.object());
}

void Project::save(const QString &path) const
{
    QJsonDocument doc(toJSON());
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();
}

QJsonObject Project::toJSON() const
{
    QJsonArray eventTypes;
    for (auto it = mEventTypes.begin(); it != mEventTypes.end(); it++) {
        eventTypes.push_back(it->get()->toJSON());
    }

    QJsonArray timelines;
    for (auto it = mTimelines.begin(); it != mTimelines.end(); it++) {
        timelines.push_back(it->get()->toJSON());
    }

    QJsonObject project;
    project["eventTypes"] = eventTypes;
    project["timelines"] = timelines;
    return project;
}

void Project::fromJSON(const QJsonObject& project)
{
    mTimelines.clear();
    mEventTypes.clear();

    const QJsonArray& eventTypes = project["eventTypes"].toArray();
    for (auto it = eventTypes.begin(); it != eventTypes.end(); it++) {
        auto eventType = std::make_shared<EventType>();
        eventType->fromJSON(it->toObject());
        mEventTypes.push_back(eventType);
    }

    const QJsonArray& timelines = project["timelines"].toArray();
    for (auto it = timelines.begin(); it != timelines.end(); it++) {
        auto timeline = std::make_shared<Timeline>();
        timeline->fromJSON(it->toObject());
        mTimelines.push_back(timeline);
    }
}

void Project::setEventTypes(const EventTypeVector& eventTypes)
{
    // TODO: update timelines
    mEventTypes = eventTypes;
}

std::shared_ptr<Timeline> Project::createTimeline()
{
    auto timeline = std::make_shared<Timeline>();
    mTimelines.push_back(timeline);
    return timeline;
}

void Project::removeTimeline(const std::shared_ptr<Timeline>& timeline)
{
    mTimelines.removeAll(timeline);
}

std::shared_ptr<EventType> Project::eventType(const QString &name)
{
    return *std::find_if(mEventTypes.begin(), mEventTypes.end(),
                      [&](const std::shared_ptr<EventType>& et) -> bool {
        return et->name.c_str() == name;
    });
}
