#include "timeline.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "placeholderevent.h"

Timeline::Timeline()
    : mBPM(120)
{
}

std::shared_ptr<TimelineLayer> Timeline::createLayer()
{
    QString name = QString("layer_%1").arg(mLayers.size());
    mLayers.push_back(std::make_shared<TimelineLayer>(name));
    emit layerAdded(mLayers.size() - 1, mLayers.last());
    return mLayers.last();
}

void Timeline::removeLayer(int idx)
{
    mLayers.remove(idx);
    emit layerRemoved(idx);
}

void Timeline::clearLayers()
{
    while (!mLayers.empty())
        removeLayer(mLayers.size() - 1);
}

void Timeline::setBPM(double bpm)
{
    mBPM = bpm;
    emit bpmChanged(mBPM);
}

void Timeline::setBackingTrack(const QString& track)
{
    mBackingTrack = track;
    emit backingTrackChanged(track);
}

void Timeline::load(const QString& path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject timeline = doc.object();

    // backing track
    QJsonObject backingTrack = timeline["backingTrack"].toObject();
    setBackingTrack(backingTrack["path"].toString());
    setBPM(backingTrack["bpm"].toDouble());

    // layers
    clearLayers();
    QJsonArray layers = timeline["layers"].toArray();
    for (auto it = layers.begin(); it != layers.end(); it++) {
        createLayer()->fromJSON((*it).toObject());
    }
}

void Timeline::save(const QString& path) const
{
    QJsonObject timeline;

    // backing track
    QJsonObject backingTrack;
    backingTrack["path"] = mBackingTrack;
    backingTrack["bpm"] = mBPM;
    timeline["backingTrack"] = backingTrack;

    // layers
    QJsonArray layers;
    for (auto layer = mLayers.begin(); layer != mLayers.end(); layer++) {
        layers.append(layer->get()->toJSON());
    }
    timeline["layers"] = layers;

    // save document
    QJsonDocument doc(timeline);
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();
}


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


void EventList::addEvent(const std::shared_ptr<TimelineEvent>& event)
{
    Q_ASSERT(event != nullptr);
    mEvents.push_back(event);
    std::sort(mEvents.begin(), mEvents.end(),
              [](const std::shared_ptr<TimelineEvent>& e1, const std::shared_ptr<TimelineEvent>& e2) -> bool {
        return e1->time() < e2->time();
    });
    emit eventAdded(event);
}

void EventList::removeEvent(const std::shared_ptr<TimelineEvent>& event)
{
    mEvents.removeOne(event);
    emit eventRemoved(event);
}

void EventList::removeEventsInRange(double start, double end)
{
    auto events = eventsInRange(start, end);
    for (auto it = events.begin(); it != events.end(); it++) {
        removeEvent(*it);
    }
}

QVector< std::shared_ptr<TimelineEvent> > EventList::eventsInRange(double start, double end) const
{
    QVector< std::shared_ptr<TimelineEvent> > ret;
    for (int i = 0; i < mEvents.size(); i++) {
        if (mEvents[i]->time() >= end)
            break;
        if (mEvents[i]->time() >= start)
            ret.push_back(mEvents[i]);
    }
    return ret;
}

QJsonArray EventList::toJSON() const
{
    QJsonArray events;
    for (auto it = mEvents.begin(); it != mEvents.end(); it++) {
        events.append(it->get()->toJSON());
    }
    return events;
}

void EventList::fromJSON(const QJsonArray& events)
{
    for (auto it = events.begin(); it != events.end(); it++) {
        addEvent(TimelineEvent::createFromJSON((*it).toObject()));
    }
}


QJsonObject TimelineEvent::toJSON() const
{
    QJsonObject event;
    event["type"] = type();
    event["time"] = time();
    writeParametersJSON(event);
    return event;
}

std::shared_ptr<TimelineEvent> TimelineEvent::createFromJSON(const QJsonObject& event)
{
    std::shared_ptr<TimelineEvent> ev;
    QString type = event["type"].toString();
    if (type == "placeholder") {
        ev = std::make_shared<PlaceholderEvent>();
    } else {
        qDebug() << "Unknown event type " << type;
        qDebug() << event;
        return nullptr;
    }

    ev->mTime = event["time"].toDouble();
    ev->readParametersJSON(event);
    return ev;
}
