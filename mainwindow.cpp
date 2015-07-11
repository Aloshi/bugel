#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "timelinesettingsdialog.h"
#include "projectsettingsdialog.h"
#include "timeline.h"
#include "project.h"
#include "placeholderevent.h"
#include "util.h"

#include <QFileDialog>
#include <QShortcut>
#include <QDebug>
#include <QMessageBox>
#include <memory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // wire placeholder actions to create a PlaceholderEvent
    const auto placeholders = ui->menuAdd_Placeholder->actions();
    for (int i = 0; i < placeholders.size(); i++) {
        QAction* action = placeholders.at(i);
        QObject::connect(action, &QAction::triggered,
                         [this, i] {
            auto event = std::make_shared<PlaceholderEvent>(ui->timeline->cursor(), i);
            ui->timeline->addEventToCurrentLayer(event);
        });

        QObject::connect(ui->timeline, &TimelineContainer::currentLayerChanged,
                         [action](int idx) {
                action->setEnabled(idx != -1);
        });

        placeholders.at(i)->setShortcut(QKeySequence(Qt::Key_1 + i));
    }

    QObject::connect(ui->timeline, &TimelineContainer::currentLayerChanged,
                     [this](int idx) {
        statusBar()->showMessage(QString("Current layer: %1.").arg(idx));
    });

    // selection-related stuff
    // print a message when selection changes and
    // disable selection-related actions when we have no selection
    QObject::connect(ui->timeline, &TimelineContainer::currentSelectionChanged,
                     [this](const Selection& selection) {
        if (selection.state() == Selection::DONE) {
            statusBar()->showMessage(QString("Selected %1 to %2.")
                                     .arg(selection.left())
                                     .arg(selection.right()));
        }
        ui->actionDelete_Selection->setEnabled(selection.state() == Selection::DONE);
    });

    updateTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newProject()
{
    QFileInfo inputPath = QFileDialog::getSaveFileName(this, "New Project (will make subfolder)", "", "");
    if (inputPath.filePath().isEmpty() || !closeProject())
        return;

    const QString name = inputPath.fileName();
    const QString dir = inputPath.path() + "/" + name;

    Project::init(dir, name);
    statusBar()->showMessage(QString("Created project in %1.")
                             .arg(Project::get()->root()));

    newTimeline();
}

void MainWindow::openProject()
{
    QString openPath = QFileDialog::getOpenFileName(this, "Open Project", "", "Bugel Project (*.bgp)");
    if (!QFileInfo(openPath).exists() || !closeProject())
        return;

    Project::open(openPath);
    statusBar()->showMessage(QString("Opened project %1.")
                             .arg(openPath));
    updateTitle();

    const QString lastPath = Project::get()->lastOpenedTimeline();
    if (QFileInfo(lastPath).exists()) {
        openTimeline(lastPath);
    }
}

void MainWindow::openProjectSettingsDialog()
{
    if (!Project::get())
        return;

    ProjectSettingsDialog dlg(Project::get());
    if (dlg.exec() == QDialog::Accepted) {
        dlg.apply(Project::get());
    }
}

bool MainWindow::closeProject()
{
    if (!closeTimeline())
        return false;

    Project::close();
    updateTitle();
    return true;
}


void MainWindow::newTimeline()
{
    if (Project::get() == nullptr || !closeTimeline())
        return;

    ui->timeline->setTimeline(std::make_shared<Timeline>());
    mTimelinePath = "";
    updateTitle();
}

void MainWindow::openTimeline()
{
    if (Project::get() == nullptr)
        return;

    QString openPath = QFileDialog::getOpenFileName(this, "Open Timeline",
                                                    Project::get()->root() + "/timelines",
                                                    "Bugel Timeline (*.bgt)");
    openTimeline(openPath);
}

void MainWindow::openTimeline(const QString& openPath)
{
    if (Project::get() == nullptr)
        return;

    QFileInfo file(openPath);
    if (!file.exists() || !closeTimeline())
        return;

    auto timeline = std::make_shared<Timeline>();
    timeline->fromJSON(Util::readJSON(openPath));
    ui->timeline->setTimeline(timeline);
    Project::get()->setLastOpenedTimeline(file.filePath());
    mTimelinePath = openPath;
    statusBar()->showMessage(QString("Opened timeline %1.")
                             .arg(openPath));
    updateTitle();
}

void MainWindow::openTimelineSettingsDialog()
{
    if (!Project::get() || ui->timeline->timeline() == nullptr)
        return;

    TimelineSettingsDialog dlg;
    dlg.init(*ui->timeline->timeline());
    if (dlg.exec() == QDialog::Accepted) {
        dlg.apply(*ui->timeline->timeline());
    }
}

bool MainWindow::saveTimeline()
{
    if (!Project::get() || ui->timeline->timeline() == nullptr)
        return false;

    if (mTimelinePath.isEmpty())
        return saveTimelineAs();

    Util::writeJSON(ui->timeline->timeline()->toJSON(), mTimelinePath);
    Project::get()->setLastOpenedTimeline(mTimelinePath);
    statusBar()->showMessage(QString("Saved timeline to %1.")
                             .arg(mTimelinePath));
    updateTitle();
    return true;
}

bool MainWindow::saveTimelineAs()
{
    if (!Project::get() || ui->timeline->timeline() == nullptr)
        return false;

    QString path = QFileDialog::getSaveFileName(this, "Save Timeline As",
                                                 Project::get()->root() + "/timelines",
                                                 "Bugel Timeline (*.bgt)");
    if (path.isEmpty())
        return false;

    mTimelinePath = path;
    return saveTimeline();
}

void MainWindow::exportTimelineAs()
{
    if (!Project::get() || ui->timeline->timeline() == nullptr)
        return;

    QString exportPath = QFileDialog::getSaveFileName(this, "Export Timeline As", "", "JSON (*.json)");
    if (!exportPath.isEmpty()) {
        std::shared_ptr<Timeline> exportTimeline = ui->timeline->timeline()->process();
        Util::writeJSON(exportTimeline->toJSON(), exportPath);
        statusBar()->showMessage(QString("Exported timeline to %1.").arg(exportPath));
    }
}

bool MainWindow::closeTimeline()
{
    // already closed?
    if (ui->timeline->timeline() == nullptr)
        return true;

    // if timeline is dirty, prompt to save/discard/cancel
    if (ui->timeline->dirty()) {
        QMessageBox:: StandardButton res =
                QMessageBox::question(this, "Save Timeline?",
                "The open timeline has not been saved.",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                QMessageBox::Cancel);

        // user clicked cancel on the question or cancelled during the save (ambiguous)
        if (res == QMessageBox::Cancel || (res == QMessageBox::Save && !saveTimeline()))
            return false;
    }

    // ok, we can close
    ui->timeline->setTimeline(nullptr);
    updateTitle();
    return true;
}

void MainWindow::updateTitle()
{
    QString proj = Project::get() ? Project::get()->name() : "";
    QString tl = ui->timeline->timeline() ? QFileInfo(mTimelinePath).fileName() : "";

    if (!proj.isEmpty() && !tl.isEmpty()) {
        setWindowTitle(QString("%1 [%2]").arg(tl).arg(proj));
    } else if (!proj.isEmpty()) {
        setWindowTitle(QString("[%1]").arg(proj));
    } else {
        setWindowTitle("Bugel");
    }
}
