#ifndef EVENTTYPEMODEL_H
#define EVENTTYPEMODEL_H

#include <QAbstractItemModel>
#include <QString>
#include <QMetaType>
#include <QList>
#include <memory>
#include <QVariant>

struct EventProperty;
struct EventType;

// i am sorry this file exists
// so, so sorry
// so so so so sorry.

class EventTypeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    EventTypeModel(QObject* parent = 0);
    virtual ~EventTypeModel() {}

    QVariant data(const QModelIndex& index, int role) const /* override */;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const /* override */;

    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const /* override */;

    QModelIndex parent(const QModelIndex& child) const /* override */;

    int rowCount(const QModelIndex& parent = QModelIndex()) const /* override */;

    int columnCount(const QModelIndex& parent = QModelIndex()) const /* override */;

    Qt::ItemFlags flags(const QModelIndex &index) const /* override */;

    bool setData(const QModelIndex &index, const QVariant &value,
                        int role = Qt::EditRole);

    void add(const std::shared_ptr<EventType>& eventType);
    void addEventType(const QString& name);
    void addProperty(const QModelIndex& eventType, const QString& name,
                     QMetaType::Type type, const QVariant& defaultValue);
    void remove(const QModelIndex& index);

    QVector< std::shared_ptr<EventType> > toVector() const;

private:
    struct TreeNode {
        TreeNode* parent;
        QList<TreeNode> properties;

        QString name;
        QMetaType::Type type;
        QVariant defaultValue;

        TreeNode(TreeNode* p, const QString& n, QMetaType::Type t, const QVariant& d)
            : parent(p), name(n), type(t), defaultValue(d) {}
    };

    QList<TreeNode> mEventTypes;
};


#endif // EVENTTYPEMODEL_H
