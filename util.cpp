#include "util.h"

#include <QFile>
#include <QJsonDocument>

namespace Util
{

QJsonObject readJSON(const QString& path)
{
    QFile file(path);
    bool opened = file.open(QIODevice::ReadOnly);
    Q_ASSERT(opened);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    return doc.object();
}

void writeJSON(const QJsonObject json, const QString &path)
{
    QJsonDocument doc(json);
    QFile file(path);
    bool opened = file.open(QIODevice::WriteOnly);
    Q_ASSERT(opened);
    file.write(doc.toJson());
    file.close();
}

}
