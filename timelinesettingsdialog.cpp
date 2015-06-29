#include "timelinesettingsdialog.h"
#include "ui_timelinesettingsdialog.h"

#include <QFileDialog>

TimelineSettingsDialog::TimelineSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimelineSettingsDialog)
{
    ui->setupUi(this);

    mBackingTrack = findChild<QLineEdit*>("backingTrack");
    mBPM = findChild<QSpinBox*>("bpm");
}

TimelineSettingsDialog::~TimelineSettingsDialog()
{
    delete ui;
}

void TimelineSettingsDialog::init(Timeline& timeline)
{
    mBackingTrack->setText(timeline.backingTrack());
    mBPM->setValue(timeline.bpm());
}

void TimelineSettingsDialog::apply(Timeline& timeline)
{
    timeline.setBackingTrack(mBackingTrack->text());
    timeline.setBPM(mBPM->value());
}

void TimelineSettingsDialog::browseBackingTrack()
{
    const QString path = QFileDialog::getOpenFileName(this,
                         "Backing Track", "", "Audio Files (*.wav *.mp3 *.ogg *.flac)");
    if (!path.isEmpty()) {
        mBackingTrack->setText(path);
    }
}
