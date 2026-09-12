#include <stdio.h>
#include "attack.h"


void attackShip(int attackerID, int targetID, float power)
{
    printf("Attack Started\n");

    printf("Attacker Ship ID : %d\n", attackerID);

    printf("Target Ship ID : %d\n", targetID);

    printf("Attack Power : %.2f\n", power);

    printf("Attack Completed\n");
}
