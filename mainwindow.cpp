#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "noteevent.h"

#include <QShortcut>
#include <QDebug>

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
            auto event = std::make_shared<NoteEvent>(mTimelineContainer->cursor(), i);
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

    QObject::connect(deleteSelectionAction, &QAction::triggered,
                     mTimelineContainer, &TimelineContainer::removeSelectionInCurrentLayer);
    deleteSelectionAction->setShortcut(QKeySequence(Qt::Key_Delete));
}

MainWindow::~MainWindow()
{
    delete ui;
}
