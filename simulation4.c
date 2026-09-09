#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "simulation4.h"
#include "damage.h"


#define MAX_SHIPS 5


struct EnemyShip
{
    int id;
    char type[3];

    float x;
    float y;

    float impactPower;
    float firingTime;
};



float calculateDistance4(float x1,float y1,float x2,float y2)
{
    return sqrt(
        (x1-x2)*(x1-x2)+
        (y1-y2)*(y1-y2)
    );
}



void runSimulation4()
{

    printf("\n===== PART 2-B SIMULATION =====\n");


    float bx,by;


    printf("Enter Battleship X position: ");
    scanf("%f",&bx);


    printf("Enter Battleship Y position: ");
    scanf("%f",&by);



    struct EnemyShip ships[MAX_SHIPS] =
    {

        {1,"EA",20,30,0.08,3},

        {2,"EB",60,40,0.06,5},

        {3,"EC",80,70,0.07,4},

        {4,"ED",30,80,0.05,6},

        {5,"EE",90,20,0.04,2}

    };



    float health = 100;

    int destroyed = 0;



    FILE *file;

    file=fopen("simulation4_result.txt","a");



    printf("\nChecking Enemy Ships...\n");



    int order=1;



    for(int i=0;i<MAX_SHIPS;i++)
    {

        float distance =
        calculateDistance4(
            bx,
            by,
            ships[i].x,
            ships[i].y
        );



        printf("\nShip ID : %d\n",
        ships[i].id);


        printf("Ship Type : %s\n",
        ships[i].type);


        printf("Distance : %.2f\n",
        distance);



        if(distance <= 50)
        {

            printf("Inside Attack Range\n");


            printf("\nAttack Order : %d\n",
            order);


            printf("Enemy Ship ID : %d\n",
            ships[i].id);



            printf("Continuous Firing\n");


            printf("Firing Interval : %.2f\n",
            ships[i].firingTime);



            // 3 continuous shots

            for(int shot=1; shot<=3; shot++)
            {

                if(destroyed==1)
                    break;



                printf("\nShot %d\n",
                shot);


                printf("Applying Attack...\n");


                float oldHealth = health;


                applyDamage(
                    &health,
                    &destroyed,
                    ships[i].impactPower
                );


                float damageTaken = oldHealth - health;


                printf("Damage Applied : %.2f\n",
                damageTaken);


                printf("Remaining Health : %.2f\n",
                health);



                if(file!=NULL)
                {

                    fprintf(file,
                    "Attack Order : %d\n",
                    order);


                    fprintf(file,
                    "Enemy Ship ID : %d\n",
                    ships[i].id);


                    fprintf(file,
                    "Shot Number : %d\n",
                    shot);


                    fprintf(file,
                    "Firing Interval : %.2f\n",
                    ships[i].firingTime);


                    fprintf(file,
                    "Damage Applied : %.2f\n",
                    damageTaken);


                    fprintf(file,
                    "Remaining Health : %.2f\n",
                    health);


                    fprintf(file,
                    "----------------------\n");

                }


            }


            order++;


            if(destroyed==1)
            {
                printf("Battleship Destroyed\n");
                break;
            }


        }

        else
        {
            printf("Out of Range\n");
        }


    }



    if(file!=NULL)
        fclose(file);



    printf("\nSimulation 2B Completed\n");

}
