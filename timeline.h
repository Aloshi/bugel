#ifndef TIMELINE
#define TIMELINE

#include <QVector>
#include <memory>
#include <QAbstractItemModel>

class TimelineEvent
{
public:
    TimelineEvent() : mTime(0) {}
    TimelineEvent(double t) : mTime(t) {}
    virtual ~TimelineEvent() {}

    inline bool operator<(const TimelineEvent& rhs) {
        return time() < rhs.time();
    }

    inline double time() const { return mTime; }

private:
    double mTime;
};



class TimelineLayer : public QObject
{
    Q_OBJECT
public:
    TimelineLayer(const QString& name = "Untitled");

    inline const QString& name() const { return mName; }

    void addEvent(const std::shared_ptr<TimelineEvent>& event);

    QVector< std::shared_ptr<TimelineEvent> > eventsInRange(double start, double end);

private:
    QString mName;
    QVector< std::shared_ptr<TimelineEvent> > mEvents;
};



class Timeline : public QObject
{
    Q_OBJECT

public:
    Timeline();
    void createLayer();

signals:
    void layerAdded(int idx, std::shared_ptr<TimelineLayer> layer);
    void layerRemoved(int idx);

private:
    QVector< std::shared_ptr<TimelineLayer> > mLayers;
};

#endif // TIMELINE

