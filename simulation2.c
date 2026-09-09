#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "simulation1.h"
#include "attack.h"
#include "damage.h"

#define MAX_POINTS 50


static float randomPosition(float D)
{
    return ((float)rand() / RAND_MAX) * D;
}


struct Point
{
    float x;
    float y;
};



int checkVerticalAngle(float angle)
{
    float minAngle = 30;

    if(angle >= minAngle && angle <= 90)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



void runSimulation2()
{
    struct Point points[MAX_POINTS];

    int k;
    int i;

    float D;
    float angle;


    // Part 1-C
    float health = 100.0;
    int destroyed = 0;

    // EA ship impact power = 8%
    float impactPower = 0.08;

    float totalImpact = 0;



    printf("\n===== PART 1-C : SIMULATION 2 =====\n");



    printf("Enter battlefield size D: ");
    scanf("%f",&D);


    printf("Enter number of movement points: ");
    scanf("%d",&k);



    if(k > MAX_POINTS)
    {
        k = MAX_POINTS;
    }



    FILE *file;

    file = fopen("simulation2_result.txt","w");


    if(file == NULL)
    {
        printf("File cannot be created\n");
        return;
    }



    printf("\nGenerated Battleship Path Points\n");
    printf("-------------------------------\n");



    for(i=0;i<k;i++)
    {
        points[i].x = randomPosition(D);
        points[i].y = randomPosition(D);


        printf("Point %d : (%.2f , %.2f)\n",
        i+1,
        points[i].x,
        points[i].y);
    }




    printf("\nRunning Simulation 2C...\n");




    for(i=0;i<k;i++)
    {

        printf("\nSimulation %d\n",i+1);



        printf("Battleship moved to : %.2f , %.2f\n",
        points[i].x,
        points[i].y);



        angle = (float)(rand()%100);



        printf("Gun Vertical Angle : %.2f\n",angle);




        if(checkVerticalAngle(angle))
        {

            printf("Angle allowed\n");

            printf("Applying attack...\n");



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
            }
            else
            {
                printf("Battleship survived\n");
            }


        }
        else
        {

            printf("Angle not allowed\n");
            printf("Attack failed\n");

        }





        fprintf(file,
        "Simulation %d\n",
        i+1);



        fprintf(file,
        "Battleship Position : %.2f %.2f\n",
        points[i].x,
        points[i].y);



        fprintf(file,
        "Vertical Angle : %.2f\n",
        angle);



        fprintf(file,
        "Remaining Health : %.2f\n",
        health);



        fprintf(file,
        "Cumulative Impact : %.2f%%\n",
        totalImpact);



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
        "----------------------\n");




        if(destroyed == 1)
        {
            break;
        }


    }



    fclose(file);



    printf("\nSimulation 2C Completed\n");

}
