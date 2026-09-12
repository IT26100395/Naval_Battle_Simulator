#include <stdio.h>

#include "simulation.h"
#include "attack.h"
#include "damage.h"


void startSimulation()
{

    printf("\n===== Naval Battle Simulation =====\n");


    printf("\nCreating Battlefield...\n");
    printf("Battlefield Created Successfully\n");


    printf("\nLoading Ships...\n");
    printf("Ship 1 Loaded\n");
    printf("Ship 2 Loaded\n");
    printf("Ship 3 Loaded\n");


    printf("\n===== Battle Started =====\n");


    // Attack Round 1

    printf("\nAttack Round 1\n");

    attackShip(1, 2, 1.0);


    printf("\nApplying Damage...\n");

    float ship2Health = 100;
    int ship2Destroyed = 0;

    applyDamage(&ship2Health, &ship2Destroyed, 50);



    // Attack Round 2

    printf("\nAttack Round 2\n");

    attackShip(2, 3, 0.8);


    printf("\nApplying Damage...\n");

    float ship3Health = 100;
    int ship3Destroyed = 0;

    applyDamage(&ship3Health, &ship3Destroyed, 40);



    // Status

    printf("\nChecking Status...\n");


    if(ship2Destroyed == 1)
    {
        printf("Ship 2 : Destroyed\n");
    }
    else
    {
        printf("Ship 2 : Active\n");
    }


    if(ship3Destroyed == 1)
    {
        printf("Ship 3 : Destroyed\n");
    }
    else
    {
        printf("Ship 3 : Active\n");
    }



    printf("\n===== Simulation Completed =====\n");

}
