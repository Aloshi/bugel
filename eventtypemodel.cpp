#include "eventtypemodel.h"

#include "timelineevent.h"
#include <QDebug>

// i am sorry this file exists
// so, so sorry
// so so so so sorry.

EventTypeModel::EventTypeModel(QObject* parent) : QAbstractItemModel(parent)
{
}

QVariant EventTypeModel::data(const QModelIndex& index, int role) const /* override */
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const TreeNode* node = (TreeNode*)index.internalPointer();
    if (index.parent().isValid()) {
        // property
        switch (index.column()) {
        case 0: return node->name;
        case 1: return QVariant((int)node->type);
        case 2: return node->defaultValue;
        }
    } else {
        // event type
        if (index.column() == 0) {
            return node->name;
        }
    }

    return QVariant();
}

QVariant EventTypeModel::headerData(int section, Qt::Orientation orientation,
                    int role) const /* override */
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
        case 0: return "Name";
        case 1: return "Type";
        case 2: return "Default Value";
        }
    }

    return QVariant();
}

QModelIndex EventTypeModel::index(int row, int column,
                  const QModelIndex& parent) const /* override */
{
    // property
    if (parent.isValid()) {
        TreeNode* parentNode = (TreeNode*)parent.internalPointer();
        if (row < 0 || row >= parentNode->properties.size())
            return QModelIndex();

        return createIndex(row, column, (TreeNode*) &parentNode->properties.at(row));
    }

    // event type
    if (row < 0 || row >= mEventTypes.size())
        return QModelIndex();

    return createIndex(row, column, (TreeNode*) &mEventTypes.at(row));
}

QModelIndex EventTypeModel::parent(const QModelIndex& child) const /* override */
{
    TreeNode* childNode = (TreeNode*)child.internalId();
    for (int i = 0; i < mEventTypes.size(); i++) {
        if (&mEventTypes.at(i) == childNode->parent)
            return createIndex(i, 0, childNode->parent);
    }

    return QModelIndex();
}

int EventTypeModel::rowCount(const QModelIndex& parent) const /* override */
{
    if (parent.isValid()) {
        TreeNode* parentNode = (TreeNode*)parent.internalPointer();
        return parentNode->properties.size();
    } else {
        return mEventTypes.size();
    }
}

int EventTypeModel::columnCount(const QModelIndex& /*parent*/) const /* override */
{
    return 3;
}

Qt::ItemFlags EventTypeModel::flags(const QModelIndex &index) const /* override */
{
    if (!index.isValid())
        return 0;

    if (index.parent().isValid() || index.column() == 0)
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return 0;
}

bool EventTypeModel::setData(const QModelIndex &index, const QVariant &value,
                    int /*role*/)
{
    TreeNode* node = (TreeNode*)index.internalPointer();
    switch (index.column()) {
    case 0: // name
        node->name = value.toString();
        return true;
    case 1: // type
        node->type = (QMetaType::Type)value.toInt();
        return true;
    case 2: // value
        node->defaultValue = value;
        return true;
    }
    return false;
}

QVector< std::shared_ptr<EventType> > EventTypeModel::toVector() const
{
    QVector< std::shared_ptr<EventType> > vec;
    for (auto evIt = mEventTypes.begin(); evIt != mEventTypes.end(); evIt++) {
        std::shared_ptr<EventType> eventType = std::make_shared<EventType>();
        eventType->name = evIt->name.toStdString();

        for (auto propIt = evIt->properties.begin(); propIt != evIt->properties.end(); propIt++) {
            eventType->properties[propIt->name] = EventProperty(propIt->type, propIt->defaultValue);
        }
        vec.push_back(eventType);
    }
    return vec;
}

void EventTypeModel::add(const std::shared_ptr<EventType>& eventType)
{
    mEventTypes.push_back(TreeNode(
            nullptr,
            eventType->name.c_str(),
            QMetaType::UnknownType,
            QVariant()
    ));
    for (auto it = eventType->properties.begin(); it != eventType->properties.end(); it++) {
        mEventTypes.back().properties.push_back(TreeNode(
            &mEventTypes.back(),
            it.key(),
            it->type,
            it->defaultValue
        ));
    }
    emit dataChanged(createIndex(mEventTypes.size() - 1, 0, &mEventTypes.back()),
                     createIndex(mEventTypes.size() - 1, 0, &mEventTypes.back()));
}

void EventTypeModel::addEventType(const QString &name)
{
    beginInsertRows(QModelIndex(), mEventTypes.size(), mEventTypes.size());
    mEventTypes.push_back(TreeNode(
            NULL,
            name,
            QMetaType::UnknownType,
            QVariant()
    ));
    endInsertRows();
}

void EventTypeModel::addProperty(const QModelIndex &eventType, const QString &name,
                                 QMetaType::Type type, const QVariant &defaultValue)
{
    if (!eventType.isValid() || eventType.parent().isValid())
        return;

    TreeNode* node = (TreeNode*)eventType.internalPointer();
    beginInsertRows(eventType, node->properties.size(),
                    node->properties.size());
    node->properties.push_back(TreeNode(
            node,
            name,
            type,
            defaultValue
    ));
    endInsertRows();
}

void EventTypeModel::remove(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    if (index.parent().isValid())
    {
        // property
        TreeNode* node = (TreeNode*)index.internalPointer();
        TreeNode* parent = node->parent;
        for (auto it = parent->properties.begin(); it != parent->properties.end(); it++) {
            if (&*it == node) {
                beginRemoveRows(index.parent(), index.row(), index.row());
                parent->properties.erase(it);
                endRemoveRows();
                break;
            }
        }
    } else {
        // event type
        for (auto it = mEventTypes.begin(); it != mEventTypes.end(); it++) {
            if (&*it == index.internalPointer()) {
                beginRemoveRows(index, index.row(), index.row());
                mEventTypes.erase(it);
                endRemoveRows();
                break;
            }
        }
    }
}
