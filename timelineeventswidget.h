#ifndef TIMELINEEVENTSWIDGET_H
#define TIMELINEEVENTSWIDGET_H

#include "timeline.h"

#include <QWidget>

class TimelineEventsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimelineEventsWidget(QWidget *parent = 0);

protected:
    double pxPerSecond() const;

    void paintEvent(QPaintEvent* ev) /* override */;
    void keyPressEvent(QKeyEvent* ev) /* override */;

signals:

public slots:
    void setEventSource(TimelineLayer* layer);
    void setViewport(double startTime, double length);
    void setCursor(double time);

private:
    TimelineLayer* mLayer;

    double mCursor;
    double mViewOffset;
    double mViewLength;
};

#endif // TIMELINEEVENTSWIDGET_H
