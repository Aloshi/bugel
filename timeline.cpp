#include "timeline.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "luascriptcontext.h"

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

std::shared_ptr<Timeline> Timeline::process() const
{
    std::shared_ptr<Timeline> timeline = std::make_shared<Timeline>();

    // copy timeline values
    timeline->setBackingTrack(backingTrack());
    timeline->setBPM(bpm());

    for (auto sourceLayer = mLayers.begin(); sourceLayer != mLayers.end(); sourceLayer++) {
        std::shared_ptr<TimelineLayer> newLayer = timeline->createLayer();
        newLayer->setName(sourceLayer->get()->name());

        LuaScriptContext script(sourceLayer->get()->script());
        script.process(sourceLayer->get(), newLayer.get());
    }

    return timeline;
}
