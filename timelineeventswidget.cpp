#include "timelineeventswidget.h"

#include <QPainter>
#include <QKeyEvent>
#include <QDebug>

#include "timeline.h"

TimelineEventsWidget::TimelineEventsWidget(QWidget *parent) : QWidget(parent)
{
    mLayer = NULL;

    mCursor = 0;
    mViewOffset = 0;
    mViewLength = 120;

    setFocusPolicy(Qt::StrongFocus);
}

void TimelineEventsWidget::setEventSource(TimelineLayer* layer)
{
    mLayer = layer;
    update();
}

void TimelineEventsWidget::setViewport(double startTime, double length)
{
    mViewOffset = startTime;
    mViewLength = length;
    update();
}

void TimelineEventsWidget::setCursor(double time)
{
    mCursor = time;
    update();
}

double TimelineEventsWidget::pxPerSecond() const
{
    return size().width() / mViewLength;
}

void TimelineEventsWidget::keyPressEvent(QKeyEvent* ev)
{
    if (ev->key() == Qt::Key_1) {
        std::shared_ptr<TimelineEvent> event = std::make_shared<TimelineEvent>(mCursor);
        mLayer->addEvent(event);
        update();
    }
}

void TimelineEventsWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.fillRect(QRectF(0, 0, size().width(), size().height()),
                     QBrush(hasFocus()
                            ? QColor(44, 44, 44)
                            : QColor(33, 33, 33)));

    if (!mLayer)
        return;

    auto events = mLayer->eventsInRange(mViewOffset, mViewOffset + mViewLength);
    for (int i = 0; i != events.size(); i++) {
        const auto& event = events.at(i);
        const double px = (event->time() - mViewOffset) * pxPerSecond();
        painter.fillRect(QRectF(px - 5, 0, 10, 10),
                         QBrush(QColor(255, 0, 0)));
    }

    painter.fillRect(QRectF((mCursor - mViewOffset) * pxPerSecond() - 1, 0, 2, size().height()),
                     QBrush(QColor(55, 55, 55)));
}
