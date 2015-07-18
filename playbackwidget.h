#ifndef PLAYBACKWIDGET_H
#define PLAYBACKWIDGET_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui {
class PlaybackWidget;
}

class QPushButton;

class PlaybackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaybackWidget(QWidget *parent = 0);
    ~PlaybackWidget();

    double snapSteps() const;

signals:
    void positionChanged(double time); // in seconds
    void durationChanged(double duration); // in seconds
    void snappingChanged(double step);

public slots:
    void setMedia(const QMediaContent& media);
    void setMedia(const QString& str);
    void setPosition(double time);

    void togglePlaying();

private slots:
    void mediaPlayerPositionChanged(qint64 ms);
    void mediaPlayerDurationChanged(qint64 ms);

private:
    Ui::PlaybackWidget *ui;

    QPushButton* mPlayButton;

    QMediaPlayer mMediaPlayer;
};

#endif // PLAYBACKWIDGET_H
