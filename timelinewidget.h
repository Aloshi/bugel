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

    void paintEvent(QPaintEvent* ev) /* override */;
    void mousePressEvent(QMouseEvent* ev) /* override */;
    // void mouseReleaseEvent(QMouseEvent* ev) /* override */;
    void wheelEvent(QWheelEvent* ev) /* override */;

signals:
    void viewMoved(double offset);
    void cursorMoved(double time);
    void timeClicked(double time);

public slots:
    void setCursor(double time);
    void setLength(double length);

private:
    double mCursor; // in seconds

    double mLength; // in seconds
    double mViewOffset; // in seconds
    double mScale;
};

#endif // TIMELINEWIDGET_H
