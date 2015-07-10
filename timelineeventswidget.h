#ifndef TIMELINEEVENTSWIDGET_H
#define TIMELINEEVENTSWIDGET_H

#include "timeline.h"
#include "selection.h"

#include <QWidget>

class TimelineEventsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimelineEventsWidget(QWidget *parent = 0);

    // getters
    inline double cursor() const { return mCursor; }
    inline const Selection& selection() const { return mSelection; }

protected:
    double pxPerSecond() const;

    void paintEvent(QPaintEvent* ev) /* override */;
    void mousePressEvent(QMouseEvent* ev) /* override */;
    void mouseMoveEvent(QMouseEvent* ev) /* override */;
    void mouseReleaseEvent(QMouseEvent* ev) /* override */;
    bool event(QEvent* event) /* override */;
    // void contextMenuEvent(QContextMenuEvent* ev) /* override */;

    // hack so that clicking to gain focus doesn't affect selection
    void focusInEvent(QFocusEvent* ev) /* override */;
    void focusOutEvent(QFocusEvent* ev) /* override */;

signals:
    void selectionChanged(const Selection& selection);
    void focusGained();
    void focusLost();

public slots:
    void setEventSource(EventList* events);
    void setViewport(double startTime, double length);
    void setCursor(double time);

private:
    float pxAtTime(double time) const;
    double timeAtCursor(QMouseEvent* ev) const;
    int eventPaintRow(const std::shared_ptr<TimelineEvent>& event) const;
    QRectF eventPaintRect(const std::shared_ptr<TimelineEvent>& event) const;
    std::shared_ptr<TimelineEvent> eventAtPos(const QPointF& pos);

    EventList* mEvents;

    double mCursor;
    double mViewOffset;
    double mViewLength;
    Selection mSelection;

    // hack so that clicking to gain focus doesn't affect selection
    bool mIgnoreNextClick;
};

#endif // TIMELINEEVENTSWIDGET_H
