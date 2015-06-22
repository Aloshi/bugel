#ifndef UTIL
#define UTIL

#include <QIntegerForSize>

namespace Util
{

double nsToSec(qint64 ns); // nanoseconds -> seconds
qint64 secToNs(double sec); // seconds -> nanoseconds

}

#endif // UTIL

