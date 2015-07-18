#ifndef TIMELINELAYERWIDGET_H
#define TIMELINELAYERWIDGET_H

#include <QWidget>
#include <memory>
#include "selection.h"

namespace Ui {
class TimelineLayerWidget;
}

class TimelineLayer;
class TimelineEventsWidget;
class QLabel;
class QComboBox;
class QPushButton;

class TimelineLayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineLayerWidget(const std::shared_ptr<TimelineLayer>& layer, QWidget *parent = 0);
    ~TimelineLayerWidget();

    const Selection& selection() const;

signals:
    void selectionChanged(const Selection& selection);
    void focusGained();
    void focusLost();

public slots:
    void setViewport(double startTime, double length);
    void setCursor(double time);
    void setSnapInterval(double interval);

private slots:
    void editScript();

private:
    Ui::TimelineLayerWidget *ui;

    QLabel* mLblLayerName;
    TimelineEventsWidget* mEventsWidget;

    QComboBox* mScriptBox;
    QPushButton* mEditScriptButton;
    QPushButton* mDeleteScriptButton;

    std::shared_ptr<TimelineLayer> mLayer;
};

#endif // TIMELINELAYERWIDGET_H
