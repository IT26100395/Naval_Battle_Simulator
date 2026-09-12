#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "simulation1.h"
#include "attack.h"
#include "damage.h"

#define MAX_POINTS 50


struct Point
{
    float x;
    float y;
};


float randomPosition(float D)
{
    return ((float)rand() / RAND_MAX) * D;
}


void runSimulation1()
{
    struct Point points[MAX_POINTS];

    int k;
    int i;

    float D;

    int destroyed = 0;

    // Part 1-C
    float health = 100.0;
    float impactPower = 0.08;   // EA = 8%
    float totalImpact = 0.0;


    printf("\n===== PART 1-C : SIMULATION 1 =====\n");


    printf("Enter battlefield size D: ");
    scanf("%f",&D);


    printf("Enter number of movement points: ");
    scanf("%d",&k);


    if(k > MAX_POINTS)
    {
        k = MAX_POINTS;
    }


    FILE *file;

    file = fopen("simulation1_result.txt","w");

    if(file == NULL)
    {
        printf("File error\n");
        return;
    }



    for(i=0;i<k;i++)
    {
        points[i].x = randomPosition(D);
        points[i].y = randomPosition(D);
    }


    printf("\nGenerated Battleship Path Points\n");


    for(i=0;i<k;i++)
    {
        printf("Point %d : (%.2f , %.2f)\n",
               i+1,
               points[i].x,
               points[i].y);
    }



    printf("\nRunning Simulation 1C...\n");



    for(i=0;i<k;i++)
    {

        printf("\nSimulation %d\n",i+1);


        float distance;


        distance = sqrt(
        (points[i].x - 50)*(points[i].x - 50)
        +
        (points[i].y - 50)*(points[i].y - 50)
        );


        printf("Battleship Position : %.2f %.2f\n",
               points[i].x,
               points[i].y);


        printf("Distance from E ship : %.2f\n",distance);



        if(distance < 30)
        {

            printf("E Ship can attack Battleship\n");


            float oldHealth = health;


            applyDamage(&health,
                        &destroyed,
                        impactPower);


            float damageTaken = oldHealth - health;


            totalImpact = totalImpact + damageTaken;


            printf("Cumulative Impact : %.2f%%\n",
                   totalImpact);



            if(destroyed == 1)
            {
                printf("Battleship Destroyed\n");
                break;
            }

        }
        else
        {
            printf("No enemy in attack range\n");
        }



        fprintf(file,
        "Simulation %d\n",
        i+1);


        fprintf(file,
        "Position : %.2f %.2f\n",
        points[i].x,
        points[i].y);


        fprintf(file,
        "Distance : %.2f\n",
        distance);


        fprintf(file,
        "Remaining Health : %.2f\n",
        health);


        if(destroyed == 1)
        {
            fprintf(file,
            "Battleship Destroyed\n");
        }
        else
        {
            fprintf(file,
            "Battleship Survived\n");
        }


        fprintf(file,
        "Cumulative Impact : %.2f%%\n",
        totalImpact);


        fprintf(file,
        "--------------------\n");



        if(destroyed == 1)
            break;

    }


    fclose(file);


    printf("\nSimulation 1C Completed\n");

}
