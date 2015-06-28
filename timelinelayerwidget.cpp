#include "timelinelayerwidget.h"
#include "ui_timelinelayerwidget.h"

#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QStringListModel>

#include "timeline.h"
#include "timelineeventswidget.h"

QStringListModel gScriptsModel(QStringList(""));

TimelineLayerWidget::TimelineLayerWidget(const std::shared_ptr<TimelineLayer>& layer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimelineLayerWidget),
    mLayer(layer)
{
    ui->setupUi(this);

    mLblLayerName = findChild<QLabel*>("layer_name");
    mEventsWidget = findChild<TimelineEventsWidget*>("events");
    mScriptBox = findChild<QComboBox*>("postprocess_script");
    mEditScriptButton = findChild<QPushButton*>("btn_edit_script");
    mDeleteScriptButton = NULL;

    mLblLayerName->setText(mLayer->name());
    mEventsWidget->setEventSource(&mLayer->events());
    mScriptBox->setModel(&gScriptsModel);
    mScriptBox->setCurrentText(mLayer->script());

    QObject::connect(mLayer.get(), &TimelineLayer::scriptChanged,
                     mScriptBox, &QComboBox::setCurrentText);
    QObject::connect(mScriptBox, SIGNAL(currentIndexChanged(const QString&)),
                     mLayer.get(), SLOT(setScript(const QString&)));
}

TimelineLayerWidget::~TimelineLayerWidget()
{
    delete ui;
}

void TimelineLayerWidget::setViewport(double startTime, double length)
{
    mEventsWidget->setViewport(startTime, length);
}

void TimelineLayerWidget::setCursor(double time)
{
    mEventsWidget->setCursor(time);
}
