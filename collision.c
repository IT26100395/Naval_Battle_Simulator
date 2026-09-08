#include <stdio.h>
#include <math.h>

#include "collision.h"


int checkCollision(float x1, float y1, float x2, float y2)
{
    float distance;


    distance = sqrt((x2 - x1) * (x2 - x1) +
                    (y2 - y1) * (y2 - y1));


    if(distance < 10)
    {
        printf("Collision Detected\n");
        return 1;
    }
    else
    {
        printf("No Collision\n");
        return 0;
    }
}
