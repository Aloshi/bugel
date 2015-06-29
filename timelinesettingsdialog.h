#ifndef TIMELINESETTINGSDIALOG_H
#define TIMELINESETTINGSDIALOG_H

#include <QDialog>

#include "timeline.h"

namespace Ui {
class TimelineSettingsDialog;
}

class QLineEdit;
class QSpinBox;

class TimelineSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimelineSettingsDialog(QWidget *parent = 0);
    ~TimelineSettingsDialog();

    void init(Timeline& timeline);
    void apply(Timeline& timeline);

private slots:
    void browseBackingTrack();

private:
    Ui::TimelineSettingsDialog *ui;

    QLineEdit* mBackingTrack;
    QSpinBox* mBPM;
};

#endif // TIMELINESETTINGSDIALOG_H
