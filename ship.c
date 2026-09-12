#include <stdio.h>
#include "ship.h"


void displayShip(Ship ship)
{
    printf("Ship ID : %d\n", ship.id);
    printf("Ship Type : %c\n", ship.type);

    printf("Position : %.2f %.2f\n", ship.x, ship.y);

    printf("Velocity : %.2f\n", ship.velocity);

    printf("Angle : %.2f\n", ship.angle);

    printf("Impact Power : %.2f\n", ship.impactPower);
}