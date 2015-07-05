#ifndef TIMELINE
#define TIMELINE

#include <memory>
#include <QString>
#include <QVector>

#include "timelinelayer.h"

class Timeline : public QObject
{
    Q_OBJECT

public:
    Timeline();

    std::shared_ptr<TimelineLayer> createLayer();
    void removeLayer(int idx);
    void clearLayers();

    void setBackingTrack(const QString& path);
    void setBPM(double bpm);

    // getters
    inline const QString& backingTrack() const { return mBackingTrack; }
    inline double bpm() const { return mBPM; }
    inline std::shared_ptr<TimelineLayer> layer(int idx) { return mLayers.at(idx); }

    void load(const QString& path);
    void save(const QString& path) const;

    std::shared_ptr<Timeline> process() const;

signals:
    void layerAdded(int idx, std::shared_ptr<TimelineLayer> layer);
    void layerRemoved(int idx);

    void backingTrackChanged(const QString& track);
    void bpmChanged(double bpm);

private:
    QString mBackingTrack;
    double mBPM;
    QVector< std::shared_ptr<TimelineLayer> > mLayers;
};

#endif // TIMELINE
