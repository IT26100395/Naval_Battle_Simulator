#include <stdio.h>
#include <math.h>

#include "movement.h"

#define PI 3.14159


void moveShip(float *x, float *y, float velocity, float angle)
{
    float radians;

    float newX;
    float newY;


    radians = angle * PI / 180;


    newX = *x + velocity * cos(radians);

    newY = *y + velocity * sin(radians);


    // Battlefield boundary check (100 x 100)

    if(newX > 100)
    {
        newX = 100;
    }

    if(newY > 100)
    {
        newY = 100;
    }

    if(newX < 0)
    {
        newX = 0;
    }

    if(newY < 0)
    {
        newY = 0;
    }


    *x = newX;
    *y = newY;


    printf("Ship moved to new position\n");

    printf("New X position : %.2f\n", *x);

    printf("New Y position : %.2f\n", *y);
}
