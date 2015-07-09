#ifndef PROJECTSETTINGSDIALOG_H
#define PROJECTSETTINGSDIALOG_H

#include <QDialog>
#include <memory>

namespace Ui {
class ProjectSettingsDialog;
}

class Project;
class EventTypeModel;

class ProjectSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectSettingsDialog(const Project* from, QWidget *parent = 0);
    ~ProjectSettingsDialog();

    void init(const Project* project);
    void apply(Project* project);

private slots:
    void insertEventType();
    void insertProperty();
    void removeEventTypeOrProperty();

private:
    Ui::ProjectSettingsDialog *ui;

    EventTypeModel* mEventTypeModel;
};

#endif // PROJECTSETTINGSDIALOG_H
