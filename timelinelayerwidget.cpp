#include "timelinelayerwidget.h"
#include "ui_timelinelayerwidget.h"

#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QStringListModel>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>

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

    QObject::connect(mEventsWidget, &TimelineEventsWidget::focusGained,
                     this, &TimelineLayerWidget::focusGained);
    QObject::connect(mEventsWidget, &TimelineEventsWidget::focusLost,
                     this, &TimelineLayerWidget::focusLost);
    QObject::connect(mEventsWidget, &TimelineEventsWidget::selectionChanged,
                     this, &TimelineLayerWidget::selectionChanged);
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

const Selection& TimelineLayerWidget::selection() const
{
    return mEventsWidget->selection();
}

void TimelineLayerWidget::editScript()
{
    if (mLayer->script().isEmpty())
        return;

    const QString path = mLayer->scriptPath();
    if (!QFileInfo(path).exists()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        QString text = QString("-- %1.lua\n\n"
                               "function process(events)\n"
                               "\tfor idx, event in pairs(events) do\n"
                               "\t\t\n"
                               "\tend\n"
                               "end\n").arg(mLayer->script());
        file.write(text.toUtf8());
        file.close();
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
