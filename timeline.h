#ifndef TIMELINE
#define TIMELINE

#include <memory>
#include <QVector>
#include <QColor>

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

    virtual const char* name() const { return "TimelineEvent"; }
    virtual int editorRow() const { return 0; }
    virtual QColor editorColor() const { return QColor(255, 0, 0); }

private:
    double mTime;
};


class EventList : public QObject
{
    Q_OBJECT
public:
    void addEvent(const std::shared_ptr<TimelineEvent>& event);
    void removeEvent(const std::shared_ptr<TimelineEvent>& event);
    void removeEventsInRange(double start, double end);
    QVector< std::shared_ptr<TimelineEvent> > eventsInRange(double start, double end) const;

signals:
    void eventAdded(const std::shared_ptr<TimelineEvent>& event);
    void eventRemoved(const std::shared_ptr<TimelineEvent>& event);

private:
    QVector< std::shared_ptr<TimelineEvent> > mEvents;
};

class TimelineLayer : public QObject
{
    Q_OBJECT
public:
    TimelineLayer(const QString& name = "Untitled");

    // getters
    inline const QString& name() const { return mName; }
    inline const QString& script() const { return mScript; }
    inline const EventList& events() const { return mEvents; }
    inline EventList& events() { return mEvents; }

signals:
    void scriptChanged(const QString& script);

public slots:
    void setScript(const QString& name);

private:
    QString mName;
    QString mScript;
    EventList mEvents;
};

class Timeline : public QObject
{
    Q_OBJECT

public:
    Timeline();
    void createLayer();

    inline std::shared_ptr<TimelineLayer> layer(int idx) { return mLayers.at(idx); }

signals:
    void layerAdded(int idx, std::shared_ptr<TimelineLayer> layer);
    void layerRemoved(int idx);

private:
    QVector< std::shared_ptr<TimelineLayer> > mLayers;
};

#endif // TIMELINE

