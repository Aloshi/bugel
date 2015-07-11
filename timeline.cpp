#include "timeline.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>

#include "luascriptcontext.h"
#include "project.h"

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

QJsonObject Timeline::toJSON() const
{
    // layers
    QJsonArray layers;
    for (auto layer = mLayers.begin(); layer != mLayers.end(); layer++) {
        layers.append(layer->get()->toJSON());
    }

    QJsonObject timeline;

    // can we make it backing track relative?
    if (Project::get() != nullptr) {
        QDir dir(Project::get()->root());
        QString relPath = dir.relativeFilePath(mBackingTrack);

        // if it goes out of the project directory, forget it
        if (relPath.startsWith(".."))
            timeline["backingTrack"] = mBackingTrack;
        else
            timeline["backingTrack"] = relPath;
    } else {
        timeline["backingTrack"] = mBackingTrack;
    }

    timeline["bpm"] = mBPM;
    timeline["layers"] = layers;
    return timeline;
}

void Timeline::fromJSON(const QJsonObject& timeline)
{
    setBackingTrack(timeline["backingTrack"].toString());

    if (Project::get() != nullptr && !QFileInfo(mBackingTrack).exists()) {
        QDir dir(Project::get()->root());
        QFileInfo path = dir.filePath(mBackingTrack);
        if (path.exists())
            setBackingTrack(path.filePath());
    }

    setBPM(timeline["bpm"].toDouble());

    // layers
    clearLayers();
    const QJsonArray& layers = timeline["layers"].toArray();
    for (auto it = layers.begin(); it != layers.end(); it++) {
        createLayer()->fromJSON((*it).toObject());
    }
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
