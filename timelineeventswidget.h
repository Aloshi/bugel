#ifndef TIMELINEEVENTSWIDGET_H
#define TIMELINEEVENTSWIDGET_H

#include "timeline.h"

#include <QWidget>

class Selection
{
public:
    enum State {
        EMPTY,
        DRAGGING,
        DONE
    };

    Selection() : mState(EMPTY), mStart(0), mEnd(0) {}

    inline void setStart(double start) {
        mStart = start;
        mState = DRAGGING;
    }
    inline void setEnd(double end) {
        mEnd = end;
        mState = DRAGGING;
    }
    inline void setDone() { mState = DONE; }
    inline void reset() { mState = EMPTY; }

    inline State state() const { return mState; }
    inline double start() const { return mStart; }
    inline double end() const { return mEnd; }
    inline double left() const { return std::min(mStart, mEnd); }
    inline double right() const { return std::max(mStart, mEnd); }
    inline double length() const { return right() - left(); }

private:
    State mState;
    double mStart;
    double mEnd;
};

class TimelineEventsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimelineEventsWidget(QWidget *parent = 0);

protected:
    double pxPerSecond() const;

    void paintEvent(QPaintEvent* ev) /* override */;
    void mousePressEvent(QMouseEvent* ev) /* override */;
    void mouseMoveEvent(QMouseEvent* ev) /* override */;
    void mouseReleaseEvent(QMouseEvent* ev) /* override */;
    void keyPressEvent(QKeyEvent* ev) /* override */;
    void contextMenuEvent(QContextMenuEvent* ev) /* override */;

    // hack so that clicking to gain focus doesn't affect selection
    void focusInEvent(QFocusEvent* ev) /* override */;

public slots:
    void setEventSource(EventList* events);
    void setViewport(double startTime, double length);
    void setCursor(double time);
    void deleteSelection();

private:
    float pxAtTime(double time) const;
    double timeAtCursor(QMouseEvent* ev) const;

    std::shared_ptr<TimelineEvent> makeEvent(double time, int keyNum) const;

    EventList* mEvents;

    double mCursor;
    double mViewOffset;
    double mViewLength;
    Selection mSelection;

    // hack so that clicking to gain focus doesn't affect selection
    bool mIgnoreNextClick;
};

#endif // TIMELINEEVENTSWIDGET_H
