#include "playbackwidget.h"
#include "ui_playbackwidget.h"

#include <QPushButton>

#include "util.h"

PlaybackWidget::PlaybackWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaybackWidget)
{
    ui->setupUi(this);

    mPlayButton = findChild<QPushButton*>("btn_play");

    QObject::connect(mPlayButton, &QPushButton::clicked,
                     this, &PlaybackWidget::togglePlaying);

    QObject::connect(&mMediaPlayer, &QMediaPlayer::positionChanged,
                     this, &PlaybackWidget::mediaPlayerPositionChanged);

//    QObject::connect(&mMediaPlayer, &QMediaPlayer::mediaChanged,
//                     this, &PlaybackWidget::mediaChanged);
    QObject::connect(&mMediaPlayer, &QMediaPlayer::durationChanged,
                     this, &PlaybackWidget::mediaPlayerDurationChanged);

    mMediaPlayer.setNotifyInterval(0);
}

PlaybackWidget::~PlaybackWidget()
{
    delete ui;
}

void PlaybackWidget::setMedia(const QMediaContent &media)
{
    mMediaPlayer.setMedia(media);
}

void PlaybackWidget::setPosition(double time)
{
    mMediaPlayer.setPosition(Util::secToNs(time));
}

void PlaybackWidget::togglePlaying()
{
    if (mMediaPlayer.state() == QMediaPlayer::PlayingState) {
        mMediaPlayer.pause();
        mPlayButton->setText("Play");
    } else {
        mMediaPlayer.play();
        mPlayButton->setText("Stop");
    }
}

void PlaybackWidget::mediaPlayerPositionChanged(qint64 ms)
{
    emit positionChanged(ms / 1000.0);
}

void PlaybackWidget::mediaPlayerDurationChanged(qint64 ms)
{
    emit durationChanged(ms / 1000.0);
}
