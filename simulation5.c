#include <stdio.h>
#include <math.h>
#include "simulation5.h"
#include "ship.h"
#include "damage.h"


double calculateDegradedPower(double initialPower, int shot, double gamma)
{
    return initialPower * pow(shot, -gamma);
}


void runSimulation5()
{
    printf("\n===== PART 2-C SIMULATION =====\n");

    double battleshipHealth = 100.0;

    int ships = 5;

    char shipType[5][3] = {
        "EA",
        "EB",
        "EC",
        "ED",
        "EE"
    };


    double impactPower[5] = {
        0.08,
        0.06,
        0.07,
        0.05,
        0.04
    };


    double gamma[5] = {
        0.20,
        0.25,
        0.30,
        0.35,
        0.40
    };


    int firingInterval[5] = {
        3,
        5,
        4,
        6,
        2
    };


    FILE *fp;

    fp = fopen("simulation5_result.txt","w");


    if(fp == NULL)
    {
        printf("File error\n");
        return;
    }



    for(int i=0;i<ships;i++)
    {

        printf("\nEnemy Ship ID : %d\n",i+1);
        printf("Ship Type : %s\n",shipType[i]);

        fprintf(fp,"\nEnemy Ship ID : %d\n",i+1);
        fprintf(fp,"Ship Type : %s\n",shipType[i]);


        printf("Continuous Firing\n");
        printf("Firing Interval : %d\n",
               firingInterval[i]);


        fprintf(fp,"Continuous Firing\n");
        fprintf(fp,"Firing Interval : %d\n",
               firingInterval[i]);



        for(int shot=1;shot<=3;shot++)
        {

            double currentPower =
            calculateDegradedPower(
            impactPower[i],
            shot,
            gamma[i]);


            double damage =
            currentPower * 100;



            battleshipHealth -= damage;



            if(battleshipHealth < 0)
                battleshipHealth = 0;



            printf("\nShot Number : %d\n",shot);
            printf("Impact Power : %.4f\n",
                   currentPower);

            printf("Damage Applied : %.2f\n",
                   damage);

            printf("Remaining Health : %.2f\n",
                   battleshipHealth);



            fprintf(fp,"\nShot Number : %d\n",shot);
            fprintf(fp,"Impact Power : %.4f\n",
                    currentPower);

            fprintf(fp,"Damage Applied : %.2f\n",
                    damage);

            fprintf(fp,"Remaining Health : %.2f\n",
                    battleshipHealth);



            if(battleshipHealth<=0)
            {
                printf("Battleship Destroyed\n");
                fprintf(fp,"Battleship Destroyed\n");
                break;
            }

        }


    }


    fclose(fp);


    printf("\nSimulation 5 Completed\n");

}
