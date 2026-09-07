#include <stdio.h>

#include "ship.h"
#include "battlefield.h"
#include "movement.h"
#include "attack.h"
#include "damage.h"
#include "collision.h"
#include "simulation.h"
#include "simulation1.h"
#include "simulation2.h"
#include "simulation3.h"
#include "simulation4.h"
#include "simulation5.h"

void displayShip(Ship ship);


int main()
{
    printf("Naval Battle Simulator Started\n\n");

    startSimulation();

    // Create ships array
    Ship ships[6];


    // Battleship
    ships[0].id = 1;
    ships[0].type = 'B';

    ships[0].x = 50;
    ships[0].y = 50;

    ships[0].velocity = 200;
    ships[0].angle = 45;

    ships[0].impactPower = 1.0;
    ships[0].destroyed = 0;

    ships[0].health = 100;



    // Escort Ship A
    ships[1].id = 2;
    ships[1].type = 'A';

    ships[1].x = 20;
    ships[1].y = 80;

    ships[1].velocity = 100;
    ships[1].angle = 30;

    ships[1].impactPower = 0.08;
    ships[1].destroyed = 0;

    ships[1].health = 100;


    // Escort Ship B
    ships[2].id = 3;
    ships[2].type = 'B';

    ships[2].x = 70;
    ships[2].y = 30;

    ships[2].velocity = 120;
    ships[2].angle = 40;

    ships[2].impactPower = 0.06;
    ships[2].destroyed = 0;



    // Escort Ship C
    ships[3].id = 4;
    ships[3].type = 'C';

    ships[3].x = 40;
    ships[3].y = 60;

    ships[3].velocity = 110;
    ships[3].angle = 35;

    ships[3].impactPower = 0.07;
    ships[3].destroyed = 0;



    // Escort Ship D
    ships[4].id = 5;
    ships[4].type = 'D';

    ships[4].x = 80;
    ships[4].y = 20;

    ships[4].velocity = 130;
    ships[4].angle = 50;

    ships[4].impactPower = 0.05;
    ships[4].destroyed = 0;



    // Escort Ship E
    ships[5].id = 6;
    ships[5].type = 'E';

    ships[5].x = 10;
    ships[5].y = 90;

    ships[5].velocity = 90;
    ships[5].angle = 25;

    ships[5].impactPower = 0.04;
    ships[5].destroyed = 0;



    // Display ships
    displayShip(ships[0]);
    displayShip(ships[1]);
    displayShip(ships[2]);
    displayShip(ships[3]);
    displayShip(ships[4]);
    displayShip(ships[5]);



    // Create Battlefield
    Battlefield field;

    field.width = 100;
    field.height = 100;

    displayBattlefield(field);



    // Test Movement
    printf("\nBefore Movement:\n");

    printf("X = %.2f\n", ships[0].x);
    printf("Y = %.2f\n", ships[0].y);

    moveShip(&ships[0].x, &ships[0].y, ships[0].velocity, ships[0].angle);


    // Attack Test
    printf("\nAttack Test\n");

    attackShip(ships[0].id, ships[1].id, ships[0].impactPower);


    printf("\nDamage Test\n");
    applyDamage(&ships[1].health, &ships[1].destroyed, 120);

    
    printf("\nCollision Test\n");
    checkCollision(ships[0].x, ships[0].y, ships[1].x, ships[1].y);


    runSimulation1();

    runSimulation2();

    runSimulation3();

    runSimulation4();

    runSimulation5();

    return 0;
}
