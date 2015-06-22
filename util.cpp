#include "util.h"

namespace Util
{

double nsToSec(qint64 ns)
{
    return ns * (1.0e-9);
}

qint64 secToNs(double sec)
{
    return sec / (1.0e-9);
}

}
