#ifndef TIMELINE
#define TIMELINE

#include <memory>
#include <QVector>
#include <QColor>

class QXmlStreamReader;
class QXmlStreamWriter;

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

    virtual const char* type() const = 0;
    virtual int editorRow() const { return 0; }
    virtual QColor editorColor() const { return QColor(255, 0, 0); }

    QJsonObject toJSON() const;
    static std::shared_ptr<TimelineEvent> createFromJSON(const QJsonObject& ev);

protected:
    virtual void writeParametersJSON(QJsonObject& ev) const = 0;
    virtual void readParametersJSON(const QJsonObject&) = 0;

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

    QJsonArray toJSON() const;
    void fromJSON(const QJsonArray& events);

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
