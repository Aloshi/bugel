#include "timelinesettingsdialog.h"
#include "ui_timelinesettingsdialog.h"

#include <QFileDialog>

TimelineSettingsDialog::TimelineSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimelineSettingsDialog)
{
    ui->setupUi(this);
}

TimelineSettingsDialog::~TimelineSettingsDialog()
{
    delete ui;
}

void TimelineSettingsDialog::init(Timeline& timeline)
{
    ui->backingTrack->setText(timeline.backingTrack());
    ui->bpm->setValue(timeline.bpm());
}

void TimelineSettingsDialog::apply(Timeline& timeline)
{
    timeline.setBackingTrack(ui->backingTrack->text());
    timeline.setBPM(ui->bpm->value());
}

void TimelineSettingsDialog::browseBackingTrack()
{
    const QString path = QFileDialog::getOpenFileName(this,
                         "Backing Track", ui->backingTrack->text(),
                         "Audio Files (*.wav *.mp3 *.ogg *.flac)");
    if (!path.isEmpty()) {
        ui->backingTrack->setText(path);
    }
}
