#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QWidget>
#include <QElapsedTimer>

class TimelineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimelineWidget(QWidget *parent = 0);

    inline double cursor() const { return mCursor; } // in seconds

protected:
    bool inRange(double time);

    int numSubTicks() const;
    double intervalLength() const;
    double pxPerSecond() const;

    void paintEvent(QPaintEvent* ev) /* override */;
    void mousePressEvent(QMouseEvent* ev) /* override */;
    // void mouseReleaseEvent(QMouseEvent* ev) /* override */;
    void wheelEvent(QWheelEvent* ev) /* override */;

signals:
    void viewportChanged(double startTime, double length);
    void cursorMoved(double time);
    void timeClicked(double time);

public slots:
    void setCursor(double time);
    void setLength(double length);

private:
    double mCursor; // in seconds

    double mLength; // in seconds
    double mViewOffset; // in seconds
    double mViewLength; // in seconds
};

#endif // TIMELINEWIDGET_H
