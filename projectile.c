#include <math.h>
#include "projectile.h"


float calculateRange(float velocity, float angle)
{
    float g = 9.8;

    float radians;

    float range;


    // Degree to radians conversion
    radians = angle * M_PI / 180.0;


    // Projectile motion formula
    range = (velocity * velocity * sin(2 * radians)) / g;


    return range;
}
