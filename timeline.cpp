#include "timeline.h"

#include <QDebug>

Timeline::Timeline()
{

}

void Timeline::createLayer()
{
    QString name = QString("layer_%1").arg(mLayers.size());
    mLayers.push_back(std::make_shared<TimelineLayer>(name));
    emit layerAdded(mLayers.size() - 1, mLayers.last());
}

TimelineLayer::TimelineLayer(const QString& name)
    : mName(name)
{
}
