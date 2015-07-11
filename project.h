#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include <QVector>
#include <QFileInfo>
#include <QJsonObject>

struct EventType;

typedef QVector< std::shared_ptr<EventType> > EventTypeVector;
class Project
{
public:
    static void init(const QString& directory, const QString& name);
    static void open(const QString& path);
    static Project* get();
    static void close();

    const QString& name() const { return mName; }
    QString root() const;

    std::shared_ptr<EventType> eventType(const QString& name);
    inline const EventTypeVector& eventTypes() const { return mEventTypes; }
    inline const QString& lastOpenedTimeline() const { return mLastOpenedTimeline; }

    void setEventTypes(const EventTypeVector& eventTypes);
    void setLastOpenedTimeline(const QString& timelinePath);

    void save() const;

private:
    Project();
    static Project* gCurrentProject;

    QJsonObject toJSON() const;
    void fromJSON(const QJsonObject& project);

    QFileInfo mSavePath;

    QString mName;
    EventTypeVector mEventTypes;
    QString mLastOpenedTimeline;
};

#endif // PROJECT_H
