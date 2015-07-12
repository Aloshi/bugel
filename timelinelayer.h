#ifndef TIMELINELAYER_H
#define TIMELINELAYER_H

#include <QObject>
#include <QString>
#include <QJsonObject>

#include "eventlist.h"

class TimelineLayer : public QObject
{
    Q_OBJECT
public:
    TimelineLayer(const QString& name = "Untitled");

    // getters
    inline const QString& name() const { return mName; }
    inline const QString& script() const { return mScript; }
    QString scriptPath() const;
    inline const EventList& events() const { return mEvents; }
    inline EventList& events() { return mEvents; }

    QJsonObject toJSON() const;
    void fromJSON(const QJsonObject& layer);

signals:
    void nameChanged(const QString& name);
    void scriptChanged(const QString& script);

public slots:
    void setName(const QString& name);
    void setScript(const QString& script);

private:
    QString mName;
    QString mScript;
    EventList mEvents;
};

#endif // TIMELINELAYER_H
