#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "timelinesettingsdialog.h"
#include "projectsettingsdialog.h"
#include "project.h"

#include <QFileDialog>
#include <QShortcut>
#include <QDebug>
#include <QMessageBox>

#include "placeholderevent.h"

QAction* getAction(QMenu* menu, const QString& name)
{
    const auto actions = menu->actions();
    for (auto it = actions.begin(); it != actions.end(); it++) {
        if ((*it)->objectName() == name)
            return *it;
    }
    return NULL;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mTimelineContainer = findChild<TimelineContainer*>("timeline");

    // layers menu
    mLayerMenu = menuBar()->findChild<QMenu*>("menuLayer");

    // wire placeholder actions to create a PlaceholderEvent
    QMenu* placeholdersMenu = mLayerMenu->findChild<QMenu*>("menuAdd_Placeholder");
    const auto placeholders = placeholdersMenu->actions();
    for (int i = 0; i < placeholders.size(); i++) {
        QAction* action = placeholders.at(i);
        QObject::connect(action, &QAction::triggered,
                         [this, i] {
            auto event = std::make_shared<PlaceholderEvent>(mTimelineContainer->cursor(), i);
            mTimelineContainer->addEventToCurrentLayer(event);
        });

        QObject::connect(mTimelineContainer, &TimelineContainer::currentLayerChanged,
                         [action](int idx) {
                action->setEnabled(idx != -1);
        });

        //QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_1 + i), this);
        //QObject::connect(shortcut, &QShortcut::activated,
        //                 placeholders.at(i), &QAction::trigger);
        placeholders.at(i)->setShortcut(QKeySequence(Qt::Key_1 + i));
    }

    QObject::connect(mTimelineContainer, &TimelineContainer::currentLayerChanged,
                     [this](int idx) {
        statusBar()->showMessage(QString("Current layer: %1.").arg(idx));
    });

    // selection-related stuff
    // print a message when selection changes and
    // disable selection-related actions when we have no selection
    QAction* deleteSelectionAction = getAction(mLayerMenu, "actionDelete_Selection");
    QObject::connect(mTimelineContainer, &TimelineContainer::currentSelectionChanged,
                     [this, deleteSelectionAction](const Selection& selection) {
        if (selection.state() == Selection::DONE) {
            statusBar()->showMessage(QString("Selected %1 to %2.")
                                     .arg(selection.left())
                                     .arg(selection.right()));
        }
        deleteSelectionAction->setEnabled(selection.state() == Selection::DONE);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openTimelineSettingsDialog()
{
    TimelineSettingsDialog dlg;
    dlg.init(*mTimelineContainer->timeline());
    if (dlg.exec() == QDialog::Accepted) {
        dlg.apply(*mTimelineContainer->timeline());
    }
}

void MainWindow::open()
{
    QString openPath = QFileDialog::getOpenFileName(this, "Open Timeline", "", "Bugel Timeline (*.bgl)");
    if (!openPath.isEmpty()) {
        mTimelineContainer->timeline()->load(openPath);
    }
}

void MainWindow::save()
{
    if (mSavePath.isEmpty()) {
        saveAs();
    } else {
        mTimelineContainer->timeline()->save(mSavePath);
        statusBar()->showMessage(QString("Saved to %1.").arg(mSavePath));
    }
}

void MainWindow::saveAs()
{
    mSavePath = QFileDialog::getSaveFileName(this, "Save Timeline As", "", "Bugel Timeline (*.bgl)");
    if (!mSavePath.isEmpty())
        save();
}

void MainWindow::exportAs()
{
    QString exportPath = QFileDialog::getSaveFileName(this, "Export Timeline As", "", "JSON (*.json)");
    if (!exportPath.isEmpty()) {
        std::shared_ptr<Timeline> exportTimeline = mTimelineContainer->timeline()->process();
        exportTimeline->save(exportPath);
    }
}

void MainWindow::openProjectSettingsDialog()
{
    ProjectSettingsDialog dlg(Project::get());
    if (dlg.exec() == QDialog::Accepted) {
        dlg.apply(Project::get());
    }
}
