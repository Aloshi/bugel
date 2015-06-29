#include "placeholderevent.h"

#include <QJsonObject>

const char* PlaceholderEvent::type() const
{
    return "placeholder";
}

int PlaceholderEvent::editorRow() const
{
    return std::max(mNote, 0);
}

QColor PlaceholderEvent::editorColor() const
{
    switch (mNote) {
    case 0: return QColor(255, 0, 0);
    case 1: return QColor(0, 255, 0);
    case 2: return QColor(0, 0, 255);
    case 3: return QColor(255, 255, 0);
    case 4: return QColor(255, 0, 255);
    case 5: return QColor(0, 255, 255);
    case 6: return QColor(255, 255, 255);
    default: return QColor(0, 0, 0);
    }
}

void PlaceholderEvent::writeParametersJSON(QJsonObject& event) const
{
    event["note"] = mNote;
}

void PlaceholderEvent::readParametersJSON(const QJsonObject& event)
{
    mNote = event["note"].toInt();
}
