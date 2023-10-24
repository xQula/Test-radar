#include "math2.h"
#include <math.h>

bool LineLineIntersection( const QPoint &r1, const QPoint &r2,
                           const QPoint &p1, const QPoint &p2,
                           float *rv, float *pv )
{
    float a[2][2], b[2];
    float d;

    a[0][0] = r2.x() - r1.x();
    a[0][1] = r2.y() - r1.y();

    a[1][0] =-p2.x() + p1.x();
    a[1][1] =-p2.y() + p1.y();

    b[0] = p1.x() - r1.x();
    b[1] = p1.y() - r1.y();

    d = a[0][0] * a[1][1] - a[1][0] * a[0][1];

    // Решения нет
    if (fabs(d) < 1e-4)
        return false;

    *rv = (b[0] * a[1][1] - a[1][0] * b[1]) / d;
    *pv = (a[0][0] * b[1] - a[0][1] * b[0]) / d;

    return true;
}
