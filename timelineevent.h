#ifndef TIMELINEEVENT_H
#define TIMELINEEVENT_H

#include <QMetaType>
#include <QVariant>
#include <QJsonObject>
#include <memory>

#include "bugelexception.h"

struct EventProperty
{
    QMetaType::Type type;
    QVariant defaultValue;

    EventProperty() : type(QMetaType::UnknownType) {}
    EventProperty(QMetaType::Type t, const QVariant& def) : type(t), defaultValue(def) {}
};

struct EventType
{
    // This is std::string because ScriptableEvent::type()
    // returns a const char*, and QString.toUtf8().constData() will go
    // out of scope immediately after returning - but std::string.c_str() doesn't.
    // (Although, it's probably not the best idea to rely on that either.)
    std::string name;
    QMap<QString, EventProperty> properties;

    EventType() {}
    EventType(const std::string& n, QMap<QString, EventProperty> props = QMap<QString, EventProperty>())
        : name(n), properties(props) {}
};

class TimelineEvent
{
public:
    TimelineEvent(const std::shared_ptr<EventType>& type, double time);
    virtual ~TimelineEvent() {}

    // sort by time
    inline bool operator<(const TimelineEvent& rhs) {
        return time() < rhs.time();
    }

    inline double time() const { return mTime; }

    // properties
    void setProperty(const QString& name, const QVariant& value);

    template<typename T>
    T getProperty(const QString& name) {
        auto it = mProperties.find(name);
        if (it == mProperties.end())
            throw PropertyException() << "Property " << name << " does not exist for event type " << mType->name();
        if (!it->canConvert<T>())
            throw PropertyException() << "Cannot convert property " << name << " of " << mType->name();
        return it->value<T>();
    }

    inline const QMap<QString, QVariant>& properties() const { return mProperties; }

    inline const char* type() const { return mType->name.c_str(); }

    // saving/loading
    QJsonObject toJSON() const;
    static std::shared_ptr<TimelineEvent> createFromJSON(const QJsonObject& ev);

private:
    double mTime;

    std::shared_ptr<EventType> mType;
    QMap<QString, QVariant> mProperties;
};

#endif // TIMELINEEVENT_H
