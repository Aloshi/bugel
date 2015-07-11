#ifndef UTIL
#define UTIL

#include <QString>
#include <QJsonObject>

namespace Util
{

QJsonObject readJSON(const QString& path);
void writeJSON(const QJsonObject, const QString& path);

}

#endif // UTIL

