#ifndef TIMELINE
#define TIMELINE

#include <QVector>
#include <memory>
#include <QAbstractItemModel>

typedef unsigned int EventTime;

class TimelineEvent
{
public:
    inline bool operator<(const TimelineEvent& rhs) {
        return time() < rhs.time();
    }

    inline EventTime time() const { return mTime; }

private:
    EventTime mTime;
};



class TimelineLayer : public QObject
{
    Q_OBJECT
public:
    TimelineLayer(const QString& name = "Untitled");

    inline const QString& name() const { return mName; }

    void addEvent(const std::shared_ptr<TimelineEvent>& event);

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

