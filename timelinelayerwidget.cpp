#include "timelinelayerwidget.h"
#include "ui_timelinelayerwidget.h"

#include <QLabel>

#include "timeline.h"

TimelineLayerWidget::TimelineLayerWidget(const std::shared_ptr<TimelineLayer>& layer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimelineLayerWidget),
    mLayer(layer)
{
    ui->setupUi(this);

    mLblLayerName = findChild<QLabel*>("layer_name");
    mLblLayerName->setText(mLayer->name());
}

TimelineLayerWidget::~TimelineLayerWidget()
{
    delete ui;
}
