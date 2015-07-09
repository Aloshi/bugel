#include "projectsettingsdialog.h"
#include "ui_projectsettingsdialog.h"

#include <QComboBox>
#include <QAbstractItemDelegate>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QStyleOptionComplex>
#include <QDebug>

#include "project.h"
#include "timelineevent.h"
#include "eventtypemodel.h"

class EventPropertyDelegate : public QStyledItemDelegate
{
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
                          const QModelIndex& /*index*/) const
    {
        QComboBox* editor = new QComboBox(parent);

        static const QMetaType::Type types[] = {
            QMetaType::QString,
            QMetaType::Double,
            QMetaType::Int,
            QMetaType::Bool
        };

        for (int i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
            editor->addItem(QMetaType::typeName(types[i]),
                                   QVariant((int)types[i]));
        }

        return editor;
    }

    void setEditorData(QWidget* editorGeneric, const QModelIndex& index) const
    {
        QComboBox* editor = (QComboBox*)editorGeneric;
        QString typeName = QMetaType::typeName((QMetaType::Type)index.data().toInt());
        editor->setCurrentText(typeName);
        editor->showPopup();
    }

    void setModelData(QWidget* editorGeneric, QAbstractItemModel* model, const QModelIndex &index) const
    {
        QComboBox* editor = (QComboBox*)editorGeneric;
        int type = QMetaType::type(editor->currentText().toUtf8().constData());
        qDebug() << "setting type to " << type;
        model->setData(index, type);
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const
    {
        if (index.data().isNull()) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        QStyleOptionComboBox cb;
        cb.rect = option.rect;
        cb.state = option.state;
        cb.frame = true;
        cb.currentText = QMetaType::typeName(index.data().toInt());
        QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &cb, painter);
        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &cb, painter);
    }
};

ProjectSettingsDialog::ProjectSettingsDialog(const Project* project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectSettingsDialog)
{
    ui->setupUi(this);

    mEventTypeModel = new EventTypeModel();
    ui->tree_eventType->setModel(mEventTypeModel);
    ui->tree_eventType->setItemDelegateForColumn(1, new EventPropertyDelegate());

    init(project);
}

ProjectSettingsDialog::~ProjectSettingsDialog()
{
    delete ui;
}

void ProjectSettingsDialog::init(const Project* project)
{
    for (auto it = project->eventTypes().begin(); it != project->eventTypes().end(); it++) {
        mEventTypeModel->add(*it);
    }
}

void ProjectSettingsDialog::apply(Project* project)
{
    auto newEventTypes = mEventTypeModel->toVector();
    project->setEventTypes(newEventTypes);
}

void ProjectSettingsDialog::insertEventType()
{
    mEventTypeModel->addEventType("New Event Type");
}

void ProjectSettingsDialog::insertProperty()
{
    auto current = ui->tree_eventType->currentIndex();
    if (!current.isValid())
        return;

    QModelIndex parent;
    if (current.parent().isValid())
        parent = current.parent();
    else
        parent = current;

    mEventTypeModel->addProperty(parent, "New Property", QMetaType::QString, QVariant(""));
}

void ProjectSettingsDialog::removeEventTypeOrProperty()
{
    mEventTypeModel->remove(ui->tree_eventType->currentIndex());
}
