#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 9.81
#define PI 3.14159265358979323846
#define MAX_ESCORTS 100

typedef struct
{
    int id;
    char type[3];
    char typeName[50];
    char gunName[50];
    double x;
    double y;
    double minVelocity;
    double maxVelocity;
    double minAngle;
    double maxAngle;
    double impactPower;
    int alive;
} EscortShip;

typedef struct
{
    char type;
    char name[50];
    char gunName[50];
    double x;
    double y;
    double maxVelocity;
    int alive;
} Battleship;

/* Generate random double value within [min, max] */
double randomDouble(double min, double max)
{
    double randomValue = (double)rand() / (double)RAND_MAX;
    return min + randomValue * (max - min);
}

/* Convert degree to radian */
double degreeToRadian(double degree)
{
    return degree * PI / 180.0;
}

/* Calculate Euclidean distance between two points */
double calculateDistance(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

/* 
   Maximum attack range of Battleship.
   Formula: R_max = (v^2 * sin(2 * 45 deg)) / g = v^2 / g
*/
double battleshipMaxRange(double velocity)
{
    return (velocity * velocity) / G;
}

/* 
   Calculate minimum and maximum attack range of an Escort ship.
   Formula: R = (v^2 * sin(2 * theta)) / g
*/
void escortAttackRange(EscortShip ship, double *minimumRange, double *maximumRange)
{
    double thetaL = degreeToRadian(ship.minAngle);
    double thetaH = degreeToRadian(ship.maxAngle);

    double sinLow = sin(2.0 * thetaL);
    double sinHigh = sin(2.0 * thetaH);

    double minimumSin = (sinLow < sinHigh) ? sinLow : sinHigh;
    double maximumSin;

    /* If 45 degrees is inside working angle range, sin(2*theta) reaches max 1.0 */
    if (ship.minAngle <= 45.0 && ship.maxAngle >= 45.0)
    {
        maximumSin = 1.0;
    }
    else
    {
        maximumSin = (sinLow > sinHigh) ? sinLow : sinHigh;
    }

    *minimumRange = (ship.minVelocity * ship.minVelocity * minimumSin) / G;
    *maximumRange = (ship.maxVelocity * ship.maxVelocity * maximumSin) / G;
}

/* 
   Calculate flight time of Battleship shell for target at given distance.
   t = (2 * v * sin(theta)) / g
*/
double calculateHitTime(double distance, double velocity)
{
    if (distance <= 0.0)
        return 0.0;

    double value = (G * distance) / (velocity * velocity);

    if (value > 1.0)
        return -1.0; /* Target out of range */

    double angle = 0.5 * asin(value); /* Lower angle trajectory */
    double time = (2.0 * velocity * sin(angle)) / G;

    return time;
}

/* Randomly generate dynamic attributes for Escort ship based on Table 1 */
void generateEscort(EscortShip *ship, int id, double canvasD, double battleshipMaxVelocity)
{
    ship->id = id;
    ship->alive = 1;

    ship->x = randomDouble(0.0, canvasD);
    ship->y = randomDouble(0.0, canvasD);

    int type = rand() % 5;
    double angleRange = 0.0;

    switch (type)
    {
        case 0:
            strcpy(ship->type, "EA");
            strcpy(ship->typeName, "1936A-class Destroyer");
            strcpy(ship->gunName, "SK C/34 naval gun");
            ship->impactPower = 0.08;
            angleRange = 20.0;
            ship->maxVelocity = 1.20 * battleshipMaxVelocity;
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.60 * ship->maxVelocity);
            break;

        case 1:
            strcpy(ship->type, "EB");
            strcpy(ship->typeName, "Gabbiano-class Corvette");
            strcpy(ship->gunName, "L/47 dual-purpose gun");
            ship->impactPower = 0.06;
            angleRange = 30.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;

        case 2:
            strcpy(ship->type, "EC");
            strcpy(ship->typeName, "Matsu-class Destroyer");
            strcpy(ship->gunName, "Type 89 dual-purpose gun");
            ship->impactPower = 0.07;
            angleRange = 25.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;

        case 3:
            strcpy(ship->type, "ED");
            strcpy(ship->typeName, "F-class Escort Ships");
            strcpy(ship->gunName, "SK C/32 naval gun");
            ship->impactPower = 0.05;
            angleRange = 50.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;

        default:
            strcpy(ship->type, "EE");
            strcpy(ship->typeName, "Japanese Kaibokan");
            strcpy(ship->gunName, "(4.7 inch) naval guns");
            ship->impactPower = 0.04;
            angleRange = 70.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
    }

    ship->minAngle = randomDouble(1.0, 90.0 - angleRange);
    ship->maxAngle = ship->minAngle + angleRange;
}

/* Save initial conditions to part1a_initial.txt */
void saveInitialConditions(Battleship battleship, EscortShip escorts[], int numberOfEscorts, double canvasD)
{
    FILE *file = fopen("part1a_initial.txt", "w");
    if (file == NULL)
    {
        printf("Error creating initial conditions file.\n");
        return;
    }

    fprintf(file, "PART 1-A INITIAL CONDITIONS\n");
    fprintf(file, "===========================\n\n");
    fprintf(file, "Battlefield Canvas Size: %.2f x %.2f\n\n", canvasD, canvasD);

    fprintf(file, "BATTLESHIP\n");
    fprintf(file, "Type Notation : %c\n", battleship.type);
    fprintf(file, "Name          : %s\n", battleship.name);
    fprintf(file, "Gun Name      : %s\n", battleship.gunName);
    fprintf(file, "Position      : (%.2f, %.2f)\n", battleship.x, battleship.y);
    fprintf(file, "Max Velocity  : %.2f m/s\n\n", battleship.maxVelocity);

    fprintf(file, "ESCORT SHIPS (%d total)\n", numberOfEscorts);
    fprintf(file, "========================\n\n");

    for (int i = 0; i < numberOfEscorts; i++)
    {
        fprintf(file, "Escort Index : %d\n", escorts[i].id);
        fprintf(file, "Type Notation: %s\n", escorts[i].type);
        fprintf(file, "Type Name    : %s\n", escorts[i].typeName);
        fprintf(file, "Gun Name     : %s\n", escorts[i].gunName);
        fprintf(file, "Position     : (%.2f, %.2f)\n", escorts[i].x, escorts[i].y);
        fprintf(file, "Velocity     : Min = %.2f m/s, Max = %.2f m/s\n", escorts[i].minVelocity, escorts[i].maxVelocity);
        fprintf(file, "Angle Range  : Min = %.2f deg, Max = %.2f deg\n", escorts[i].minAngle, escorts[i].maxAngle);
        fprintf(file, "Impact Power : %.2f\n\n", escorts[i].impactPower);
    }

    fclose(file);
}

/* Save final battlefield conditions to part1a_final.txt */
void saveFinalConditions(Battleship battleship, EscortShip escorts[], int numberOfEscorts)
{
    FILE *file = fopen("part1a_final.txt", "w");
    if (file == NULL)
    {
        printf("Error creating final status file.\n");
        return;
    }

    fprintf(file, "PART 1-A FINAL CONDITIONS\n");
    fprintf(file, "=========================\n\n");
    fprintf(file, "Battleship Status: %s\n\n", battleship.alive ? "ALIVE" : "DESTROYED");

    fprintf(file, "Escort Ships Status\n");
    fprintf(file, "-------------------\n");

    for (int i = 0; i < numberOfEscorts; i++)
    {
        fprintf(file, "E%d (%s): %s\n", escorts[i].id, escorts[i].type, escorts[i].alive ? "ALIVE" : "DESTROYED");
    }

    fclose(file);
}

int main()
{
    double canvasD;
    int numberOfEscorts;
    unsigned int seed;

    Battleship battleship;
    EscortShip escorts[MAX_ESCORTS];

    printf("\n=======================================\n");
    printf("     ADVANCED NAVAL SIMULATOR 1-A      \n");
    printf("=======================================\n\n");

    printf("Enter random seed value: ");
    if (scanf("%u", &seed) != 1) return 1;
    srand(seed);

    printf("Enter battlefield canvas size D: ");
    if (scanf("%lf", &canvasD) != 1 || canvasD <= 0)
    {
        printf("Invalid battlefield size.\n");
        return 1;
    }

    printf("Enter number of Escort ships (1-%d): ", MAX_ESCORTS);
    if (scanf("%d", &numberOfEscorts) != 1 || numberOfEscorts <= 0 || numberOfEscorts > MAX_ESCORTS)
    {
        printf("Invalid number of Escorts.\n");
        return 1;
    }

    printf("\nBattleship Types:\n");
    printf(" U - USS Iowa (BB-61)\n");
    printf(" M - MS King George V\n");
    printf(" R - Richelieu\n");
    printf(" S - Sovetsky Soyuz-class\n");

    printf("\nEnter Battleship type (U/M/R/S): ");
    scanf(" %c", &battleship.type);

    switch (battleship.type)
    {
        case 'U': case 'u':
            battleship.type = 'U';
            strcpy(battleship.name, "USS Iowa (BB-61)");
            strcpy(battleship.gunName, "50-caliber Mark 7 gun");
            break;
        case 'M': case 'm':
            battleship.type = 'M';
            strcpy(battleship.name, "MS King George V");
            strcpy(battleship.gunName, "(356 mm) Mark VII gun");
            break;
        case 'R': case 'r':
            battleship.type = 'R';
            strcpy(battleship.name, "Richelieu");
            strcpy(battleship.gunName, "(15 inch) Mle 1935 gun");
            break;
        case 'S': case 's':
            battleship.type = 'S';
            strcpy(battleship.name, "Sovetsky Soyuz-class");
            strcpy(battleship.gunName, "(16 inch) B-37 gun");
            break;
        default:
            printf("Invalid Battleship type.\n");
            return 1;
    }

    printf("Enter Battleship maximum shell velocity VmaxB (m/s): ");
    if (scanf("%lf", &battleship.maxVelocity) != 1 || battleship.maxVelocity <= 0)
    {
        printf("Invalid velocity.\n");
        return 1;
    }

    printf("Enter Battleship X position (0 - %.2f): ", canvasD);
    scanf("%lf", &battleship.x);
    printf("Enter Battleship Y position (0 - %.2f): ", canvasD);
    scanf("%lf", &battleship.y);

    if (battleship.x < 0 || battleship.x > canvasD || battleship.y < 0 || battleship.y > canvasD)
    {
        printf("Battleship position is outside canvas boundaries!\n");
        return 1;
    }

    battleship.alive = 1;

    /* Generate Escort Ships randomly */
    for (int i = 0; i < numberOfEscorts; i++)
    {
        generateEscort(&escorts[i], i + 1, canvasD, battleship.maxVelocity);
    }

    /* Save Initial Conditions */
    saveInitialConditions(battleship, escorts, numberOfEscorts, canvasD);
    printf("\nInitial conditions saved to 'part1a_initial.txt'.\n");

    printf("\n--------- BATTLEFIELD LAYOUT ---------\n");
    printf("Battleship %s [%c] at (%.2f, %.2f)\n\n", battleship.name, battleship.type, battleship.x, battleship.y);

    for (int i = 0; i < numberOfEscorts; i++)
    {
        printf("Escort E%d (%s) at (%.2f, %.2f)\n", escorts[i].id, escorts[i].type, escorts[i].x, escorts[i].y);
    }

    /* Step 1: Check whether any Escort ship can hit Battleship */
    int sinkingEscortIndex = -1;

    printf("\n--- STEP 1: EVALUATING ESCORT ATTACKS ---\n");
    for (int i = 0; i < numberOfEscorts; i++)
    {
        double distance = calculateDistance(escorts[i].x, escorts[i].y, battleship.x, battleship.y);
        double minRange, maxRange;

        escortAttackRange(escorts[i], &minRange, &maxRange);

        printf("E%d -> B dist: %.2f | Range: [%.2f - %.2f]", escorts[i].id, distance, minRange, maxRange);

        if (distance >= minRange && distance <= maxRange)
        {
            printf(" -> CAN HIT B\n");
            if (sinkingEscortIndex == -1)
            {
                sinkingEscortIndex = i; /* First Escort capable of hitting B sinks it */
            }
        }
        else
        {
            printf(" -> CANNOT HIT B\n");
        }
    }

    /* If Battleship sinks */
    if (sinkingEscortIndex != -1)
    {
        battleship.alive = 0;

        printf("\n=======================================\n");
        printf("       BATTLESHIP DESTROYED!           \n");
        printf("Index of Escort that sank B: E%d (%s)\n", escorts[sinkingEscortIndex].id, escorts[sinkingEscortIndex].type);
        printf("=======================================\n");

        saveFinalConditions(battleship, escorts, numberOfEscorts);
        printf("\nFinal conditions saved to 'part1a_final.txt'.\n");
        return 0;
    }

    /* Step 2: Battleship counter-attacks all Escorts within range */
    printf("\n--- STEP 2: BATTLESHIP COUNTER-ATTACK ---\n");
    printf("Battleship survived Escort attacks.\n");

    double bMaxRange = battleshipMaxRange(battleship.maxVelocity);
    printf("Battleship maximum attack range: %.2f m\n\n", bMaxRange);

    FILE *hitFile = fopen("part1a_hits.txt", "w");
    if (hitFile == NULL)
    {
        printf("Error creating hit file.\n");
        return 1;
    }

    fprintf(hitFile, "PART 1-A ESCORT SHIPS HIT BY BATTLESHIP\n");
    fprintf(hitFile, "======================================\n\n");

    int hitCount = 0;
    double battleEndTime = 0.0;

    for (int i = 0; i < numberOfEscorts; i++)
    {
        double distance = calculateDistance(battleship.x, battleship.y, escorts[i].x, escorts[i].y);

        if (distance <= bMaxRange)
        {
            escorts[i].alive = 0;
            hitCount++;

            double hitTime = calculateHitTime(distance, battleship.maxVelocity);

            printf("Battleship HIT E%d (%s) | Distance: %.2f | Time: %.2f seconds\n", escorts[i].id, escorts[i].type, distance, hitTime);
            
            fprintf(hitFile, "Index of E  : %d\n", escorts[i].id);
            fprintf(hitFile, "Type        : %s\n", escorts[i].type);
            fprintf(hitFile, "Distance    : %.2f m\n", distance);
            fprintf(hitFile, "Time to hit : %.2f seconds\n\n", hitTime);

            if (hitTime > battleEndTime)
            {
                battleEndTime = hitTime;
            }
        }
        else
        {
            printf("E%d (%s) is outside Battleship range.\n", escorts[i].id, escorts[i].type);
        }
    }

    fclose(hitFile);

    printf("\n=======================================\n");
    printf("             BATTLE RESULT             \n");
    printf("=======================================\n");
    printf("Battleship Status      : SURVIVED\n");
    printf("Escort ships hit by B  : %d\n", hitCount);
    printf("Battle end time        : %.2f seconds\n", battleEndTime);

    saveFinalConditions(battleship, escorts, numberOfEscorts);

    printf("\nResults saved to text files:\n");
    printf(" - part1a_initial.txt\n");
    printf(" - part1a_hits.txt\n");
    printf(" - part1a_final.txt\n");

    return 0;
}
