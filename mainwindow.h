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
    void openTimelineSettingsDialog();
    void openProjectSettingsDialog();
    void open();
    void save();
    void saveAs();

    void exportAs();

private:
    Ui::MainWindow *ui;

    QMenu* mLayerMenu;
    TimelineContainer* mTimelineContainer;
    QString mSavePath;
};

#endif // MAINWINDOW_H
