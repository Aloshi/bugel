#include "timelineeventswidget.h"

#include <QPainter>
#include <QKeyEvent>
#include <QMenu>
#include <QShortcut>
#include <QDebug>
#include <QToolTip>

#include "timeline.h"
#include "timelineevent.h"

TimelineEventsWidget::TimelineEventsWidget(QWidget *parent) : QWidget(parent)
{
    mEvents = NULL;

    mSnapInterval = 0;
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

bool TimelineEventsWidget::event(QEvent* ev)
{
    if (ev->type() == QEvent::ToolTip) {
        QHelpEvent* helpEvent = (QHelpEvent*)ev;
        const auto tlEvent = eventAtPos(helpEvent->pos());
        if (tlEvent)
            QToolTip::showText(helpEvent->globalPos(), tlEvent->typeName(), this,
                               eventPaintRect(tlEvent).toRect());
        ev->accept();
    }

    return QWidget::event(ev);
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

int TimelineEventsWidget::eventPaintRow(const std::shared_ptr<TimelineEvent>& event) const
{
    return (strcmp(event->typeName(), "placeholder") == 0)
            ? event->getProperty<int>("note")
            : 0;
}

#define SQUARE_WIDTH 8.0
#define SQUARE_HEIGHT 8.0

QRectF TimelineEventsWidget::eventPaintRect(const std::shared_ptr<TimelineEvent>& event) const
{
    return QRectF(pxAtTime(event->time()) - (SQUARE_WIDTH / 2.0f),
                  eventPaintRow(event) * ((size().height() - SQUARE_HEIGHT) / 10),
                  SQUARE_WIDTH, SQUARE_HEIGHT);
}

std::shared_ptr<TimelineEvent> TimelineEventsWidget::eventAtPos(const QPointF &pos)
{
    const double time = (pos.x() / (double)size().width()) * mViewLength + mViewOffset;
    const double width = ((double)SQUARE_HEIGHT / size().width()) * mViewLength;
    auto events = mEvents->eventsInRange(time - width / 2, time + width / 2);
    for (auto it = events.begin(); it != events.end(); it++) {
        if (eventPaintRect(*it).contains(pos))
            return *it;
    }
    return nullptr;
}

void TimelineEventsWidget::setSnapInterval(double snapInterval)
{
    mSnapInterval = snapInterval;
    update();
}

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

    // draw snap lines
    if (mSnapInterval != 0) {
        const double start = mViewOffset - fmod(mViewOffset, mSnapInterval);
        for (double time = start; time < mViewOffset + mViewLength; time += mSnapInterval) {
            const float x = pxAtTime(time);
            painter.fillRect(QRectF(x - 0.5f, 0, 1, height()), QColor(88, 88, 88));
        }
    }

    // draw events
    auto events = mEvents->eventsInRange(mViewOffset, mViewOffset + mViewLength);
    for (int i = 0; i != events.size(); i++) {
        const auto& event = events.at(i);

        QRectF editorRect = eventPaintRect(event);
        const int editorRow = eventPaintRow(event);
        QColor color;
        switch (editorRow) {
        case 0: color = QColor(255, 0, 0); break;
        case 1: color = QColor(0, 255, 0); break;
        case 2: color = QColor(0, 0, 255); break;
        case 3: color = QColor(255, 255, 0); break;
        case 4: color = QColor(255, 0, 255); break;
        case 5: color = QColor(0, 255, 255); break;
        case 6: color = QColor(255, 255, 255); break;
        case 7: color = QColor(255, 128, 0); break;
        case 8: color = QColor(255, 0, 128); break;
        case 9: color = QColor(76, 128, 76); break;
        default: color = QColor(255, 0, 0); break;
        }

        painter.fillRect(editorRect, QBrush(color));
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
