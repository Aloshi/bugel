#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class TimelineContainer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void newProject();
    void openProject();
    void openProjectSettingsDialog();
    bool closeProject();

    void newTimeline();
    void openTimeline();
    void openTimelineSettingsDialog();
    bool saveTimeline();
    bool saveTimelineAs();
    void exportTimelineAs();
    bool closeTimeline();

private:
    void openTimeline(const QString& path);
    void updateTitle();

    Ui::MainWindow *ui;

    QString mTimelinePath;  // path to save the currently opened timeline
};

#endif // MAINWINDOW_H
