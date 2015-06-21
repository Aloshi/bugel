#include "timelinewidget.h"

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDebug>

TimelineWidget::TimelineWidget(QWidget *parent) : QWidget(parent)
{
    mCursor = 0;

    mLength = 60;
    mViewOffset = 0;
    mScale = 1;
}

void TimelineWidget::wheelEvent(QWheelEvent* ev)
{
    mViewOffset += (int)(ev->delta() * -0.05f);
    update();
}

void TimelineWidget::mousePressEvent(QMouseEvent* ev)
{
    // 80 / 15 should be pxPerSecond
    double newCursor = ev->x() / (80.0 / 15.0) + mViewOffset;
    setCursor(newCursor);
}

void TimelineWidget::setCursor(double time)
{
    mCursor = time;
    emit cursorMoved(mCursor);
    update();
}

void TimelineWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    const double interval = 15.0;
    const int numSubTicks = (int)(interval / 5.0);
    const double pxPerInterval = 80;
    const double pxPerSecond = pxPerInterval / interval;

    int curInterval; // in seconds
    if (mViewOffset >= 0)
        curInterval = (int)(floor(mViewOffset / interval) * interval);
    else
        curInterval = (int)(ceil(mViewOffset / interval) * interval);

    double remainder = (-fmod(mViewOffset, interval)) * pxPerSecond; // in px

    static const QPen outOfRangePen(QColor(255, 0, 0));
    static const QPen inRangePen(QColor(0, 0, 0));
    static const QBrush bigTickBrush(QColor(0, 0, 0));

    // start one interval early so we get subticks on the left
    curInterval -= interval;
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
                         bigTickBrush);

        curInterval += interval;

        // draw sub-ticks
        for (int j = 1; j < numSubTicks; j++) {
            const float left = i + j * ((float)1 / numSubTicks * pxPerInterval);
            static const float subTickHeightPerc = 0.2f;
            painter.fillRect(QRectF(left, size().height() * (1 - subTickHeightPerc),
                                    1, (size().height() * subTickHeightPerc) - 1),
                             bigTickBrush);
        }
    }

    // draw cursor
    painter.fillRect(QRectF(((mCursor - mViewOffset) * pxPerSecond) - 1, 0,
                            2, size().height()), QBrush(QColor(0, 0, 20)));
}
