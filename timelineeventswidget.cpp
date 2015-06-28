#include "timelineeventswidget.h"

#include <QPainter>
#include <QKeyEvent>
#include <QMenu>
#include <QShortcut>
#include <QDebug>

#include "timeline.h"
#include "noteevent.h"

TimelineEventsWidget::TimelineEventsWidget(QWidget *parent) : QWidget(parent)
{
    mEvents = NULL;

    mCursor = 0;
    mViewOffset = 0;
    mViewLength = 60;
    mIgnoreNextClick = false;

    setFocusPolicy(Qt::StrongFocus);
}

void TimelineEventsWidget::setEventSource(EventList* events)
{
    if (mEvents) {
        mEvents->disconnect(this);
    }

    mEvents = events;
    QObject::connect(mEvents, SIGNAL(eventAdded(std::shared_ptr<TimelineEvent>)),
                     this, SLOT(update()));
    QObject::connect(mEvents, SIGNAL(eventRemoved(std::shared_ptr<TimelineEvent>)),
                     this, SLOT(update()));
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

double TimelineEventsWidget::timeAtCursor(QMouseEvent* ev) const
{
    return (ev->x() / (double)size().width()) * mViewLength + mViewOffset;
}

float TimelineEventsWidget::pxAtTime(double time) const
{
    return (time - mViewOffset) * pxPerSecond();
}

void TimelineEventsWidget::focusInEvent(QFocusEvent* ev)
{
    // if we already have a selection and are just clicking to
    // give focus, ignore the next click so we don't unselect
    if (ev->reason() == Qt::MouseFocusReason && mSelection.state() == Selection::DONE) {
        mIgnoreNextClick = true;
    }
    QWidget::focusInEvent(ev);
    if (hasFocus())
        emit focusGained();
}

void TimelineEventsWidget::focusOutEvent(QFocusEvent* ev)
{
    QWidget::focusOutEvent(ev);
    if (!hasFocus())
        emit focusLost();
}

void TimelineEventsWidget::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton) {
        if (mIgnoreNextClick) {
            mIgnoreNextClick = false;
            ev->accept();
            return;
        }
        mSelection.reset();

        const double time = timeAtCursor(ev);
        mSelection.setStart(time);
        mSelection.setEnd(time);
        update();
        ev->accept();
    }
}

void TimelineEventsWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if (mSelection.state() == Selection::DRAGGING) {
        mSelection.setEnd(timeAtCursor(ev));
        update();
        ev->accept();
    }
}

void TimelineEventsWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton && mSelection.state() == Selection::DRAGGING) {
        mSelection.setEnd(timeAtCursor(ev));
        mSelection.setDone();

        // ignore very small selections
        if (mSelection.length() < 1.0 / pxPerSecond())
            mSelection.reset();

        emit selectionChanged(mSelection);
        update();
        ev->accept();
    }
}

/*
void TimelineEventsWidget::contextMenuEvent(QContextMenuEvent* ev)
{
    QRectF selectionRect(pxAtTime(mSelection.left()), 0,
                         mSelection.length() * pxPerSecond(), size().height());

    QMenu menu;
    if (mSelection.state() == Selection::DONE && selectionRect.contains(ev->pos())) {
        // selection context menu
        menu.addAction("Delete events", this, SLOT(deleteSelection()));
    } else {
        // not selection context menu..?
        menu.addAction("there is nothing here");
    }

    menu.exec(ev->globalPos());
    ev->accept();
}*/

void TimelineEventsWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // draw background
    painter.fillRect(QRectF(0, 0, size().width(), size().height()),
                     QBrush(hasFocus()
                            ? QColor(44, 44, 44)
                            : QColor(33, 33, 33)));

    if (!mEvents)
        return;

    // draw events
    auto events = mEvents->eventsInRange(mViewOffset, mViewOffset + mViewLength);
    for (int i = 0; i != events.size(); i++) {
        const auto& event = events.at(i);

        const float width = 8;
        const float height = 8;
        const float x = pxAtTime(event->time()) - (width / 2.0f);
        const float y = event->editorRow() * ((size().height() - height) / 10);
        painter.fillRect(QRectF(x, y, width, height), QBrush(event->editorColor()));
    }


    // draw selection
    if (mSelection.state() != Selection::EMPTY) {
        painter.fillRect(QRectF(pxAtTime(mSelection.left()), 0,
                                mSelection.length() * pxPerSecond(), size().height()),
                         QBrush(QColor(66, 0, 66, 64)));
    }

    // draw cursor
    painter.fillRect(QRectF((mCursor - mViewOffset) * pxPerSecond() - 1, 0, 2, size().height()),
                     QBrush(QColor(55, 55, 55)));
}
