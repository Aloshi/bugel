#include "project.h"

#include "timeline.h"
#include "timelineevent.h"
#include "util.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QDir>

Project* Project::gCurrentProject = NULL;

void Project::open(const QString& path)
{
    close();

    gCurrentProject = new Project();
    gCurrentProject->mSavePath = path;
    gCurrentProject->fromJSON(Util::readJSON(path));
}

void Project::init(const QString& directory, const QString &name)
{
    close();

    bool ok = QDir().mkpath(directory);
    Q_ASSERT(ok);
    ok = QDir().mkpath(directory + "/scripts");
    Q_ASSERT(ok);
    ok = QDir().mkpath(directory + "/timelines");
    Q_ASSERT(ok);

    gCurrentProject = new Project();
    gCurrentProject->mName = name;
    gCurrentProject->mSavePath = directory + "/" + name + ".bgp";
    gCurrentProject->save();
}

Project* Project::get()
{
    return gCurrentProject;
}

void Project::close()
{
    if (gCurrentProject) {
        delete gCurrentProject;
        gCurrentProject = NULL;
    }
}

Project::Project()
{
    auto placeholderType = std::make_shared<EventType>("placeholder");
    placeholderType->properties["note"] = EventProperty(QMetaType::Int, QVariant(0));

    mEventTypes.push_back(placeholderType);
}

QString Project::root() const
{
    return mSavePath.path();
}

void Project::save() const
{
    Util::writeJSON(toJSON(), mSavePath.filePath());
}

QJsonObject Project::toJSON() const
{
    QJsonArray eventTypes;
    for (auto it = mEventTypes.begin(); it != mEventTypes.end(); it++) {
        eventTypes.push_back(it->get()->toJSON());
    }

    QJsonObject project;
    project["name"] = mName;
    project["lastOpenedTimeline"] = QDir(root()).relativeFilePath(mLastOpenedTimeline);
    project["eventTypes"] = eventTypes;
    return project;
}

void Project::fromJSON(const QJsonObject& project)
{
    mEventTypes.clear();

    mName = project["name"].toString();
    mLastOpenedTimeline = QDir(root()).filePath(project["lastOpenedTimeline"].toString());

    const QJsonArray& eventTypes = project["eventTypes"].toArray();
    for (auto it = eventTypes.begin(); it != eventTypes.end(); it++) {
        auto eventType = std::make_shared<EventType>();
        eventType->fromJSON(it->toObject());
        mEventTypes.push_back(eventType);
    }
}

void Project::setEventTypes(const EventTypeVector& eventTypes)
{
    mEventTypes = eventTypes;
    save();
}

void Project::setLastOpenedTimeline(const QString &timelinePath)
{
    mLastOpenedTimeline = timelinePath;
    save();
}

std::shared_ptr<EventType> Project::eventType(const QString &name)
{
    return *std::find_if(mEventTypes.begin(), mEventTypes.end(),
                      [&](const std::shared_ptr<EventType>& et) -> bool {
        return et->name.c_str() == name;
    });
}
