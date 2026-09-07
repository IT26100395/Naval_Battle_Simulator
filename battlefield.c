#include <stdio.h>
#include "battlefield.h"


void displayBattlefield(Battlefield field)
{
    printf("Battlefield Size\n");

    printf("Width  : %d\n", field.width);

    printf("Height : %d\n", field.height);
}