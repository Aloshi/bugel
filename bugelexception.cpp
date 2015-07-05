#include "bugelexception.h"

std::ostream& operator<<(std::ostream& stream, const QString& str) {
    stream << str.toStdString();
    return stream;
}
