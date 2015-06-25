#include "timelinewidget.h"

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

TimelineWidget::TimelineWidget(QWidget *parent) : QWidget(parent)
{
    mCursor = 0;

    mLength = 60;
    mViewOffset = 0;
    mViewLength = 120;
}

void TimelineWidget::wheelEvent(QWheelEvent* ev)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        // zoom
        mViewLength += (int)(ev->delta() * -0.04);
        mViewLength = std::max<double>(mViewLength, 5);
    } else {
        // scroll
        // this should really snap to intervalLength()
        double scrollAmt = (int)(ev->delta() * -0.00035 * size().width() / pxPerSecond());
        if (scrollAmt < 1 && scrollAmt > -1)
            scrollAmt = signbit((float)ev->delta()) ? 1 : -1;
        mViewOffset += scrollAmt;
    }

    emit viewportChanged(mViewOffset, mViewLength);
    update();
}

void TimelineWidget::mousePressEvent(QMouseEvent* ev)
{
    double time = ev->x() / pxPerSecond() + mViewOffset;
    emit timeClicked(time);
}

void TimelineWidget::setCursor(double time)
{
    mCursor = time;
    emit cursorMoved(mCursor);
    update();
}

void TimelineWidget::setLength(double length)
{
    mLength = length;
    update();
}

double TimelineWidget::intervalLength() const
{
    if (mViewLength >= 60)
        return 15.0;
    else if (mViewLength >= 15)
        return 5.0;
    else
        return 1.0;
}

double TimelineWidget::pxPerSecond() const
{
    return size().width() / mViewLength;
}

void TimelineWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    const int numSubTicks = (intervalLength() >= 15
                       ? (int)(intervalLength() / 5.0)
                       : (int)(intervalLength()));
    const double pxPerInterval = pxPerSecond() * intervalLength();

    int curInterval; // in seconds
    if (mViewOffset >= 0)
        curInterval = (int)(floor(mViewOffset / intervalLength()) * intervalLength());
    else
        curInterval = (int)(ceil(mViewOffset / intervalLength()) * intervalLength());

    double remainder = (-fmod(mViewOffset, intervalLength())) * pxPerSecond(); // in px

    static const QPen outOfRangePen(QColor(255, 0, 0));
    static const QPen inRangePen(QColor(0, 0, 0));

    static const QBrush inRangeBrush(QColor(0, 0, 0));
    static const QBrush outOfRangeBrush(QColor(255, 0, 0));

    // start one interval early so we get subticks on the left
    curInterval -= intervalLength();
    remainder -= pxPerInterval;

    // draw tick marks + text
    for (double i = remainder; i <= size().width(); i += pxPerInterval) {
        const int mins = abs(curInterval / 60);
        const int secs = abs(curInterval % 60);

        const QString text = QString("%1%2:%3")
                .arg((curInterval < 0) ? "-" : "")
                .arg(mins)
                .arg(QString("%1").arg(secs), 2, QLatin1Char('0'));

        painter.setPen((curInterval < 0 || curInterval >= mLength)
                       ? outOfRangePen
                       : inRangePen);

        static const float bigTickHeightPerc = 0.3f;

        const float textLeft = i - (pxPerInterval / 2.0f);
        const float textHeight = size().height() * (1 - bigTickHeightPerc);

        // draw the text
        painter.drawText(QRectF(textLeft, 0, pxPerInterval, textHeight),
                         text, QTextOption(Qt::AlignCenter));

        // draw the tick
        static const float bigTickWidth = 2;
        painter.fillRect(QRectF(i - bigTickWidth / 2,                       // x
                                size().height() * (1 - bigTickHeightPerc),  // y
                                bigTickWidth,                               // width
                                (size().height() * bigTickHeightPerc) - 1), // height
                         inRange(curInterval) ? inRangeBrush : outOfRangeBrush);

        curInterval += intervalLength();

        // draw sub-ticks
        for (int j = 1; j < numSubTicks; j++) {
            const double subTickTime = curInterval -
                    ((double)j / numSubTicks) * intervalLength();
            const float left = i + j * ((float)1 / numSubTicks * pxPerInterval);
            static const float subTickHeightPerc = 0.2f;
            painter.fillRect(QRectF(left, size().height() * (1 - subTickHeightPerc),
                                    1, (size().height() * subTickHeightPerc) - 1),
                             inRange(subTickTime) ? inRangeBrush : outOfRangeBrush);
        }
    }

    // draw cursor
    painter.fillRect(QRectF(((mCursor - mViewOffset) * pxPerSecond()) - 1, 0,
                            2, size().height()), QBrush(QColor(0, 0, 20)));
}

bool TimelineWidget::inRange(double time)
{
    return (time >= 0 && time < mLength);
}
