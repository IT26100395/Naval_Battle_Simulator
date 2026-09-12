#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 9.81
#define PI 3.14159265358979323846
#define MAX_ESCORTS 100
#define MAX_PATH_POINTS 50

typedef struct
{
    double x;
    double y;
} Point;

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

/* Random double generator */
double randomDouble(double min, double max)
{
    double randomValue = (double)rand() / (double)RAND_MAX;
    return min + randomValue * (max - min);
}

/* Degree to Radian conversion */
double degreeToRadian(double degree)
{
    return degree * PI / 180.0;
}

/* Calculate Euclidean Distance */
double calculateDistance(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

/* Calculate Escort Attack Range */
void escortAttackRange(EscortShip ship, double *minimumRange, double *maximumRange)
{
    double thetaL = degreeToRadian(ship.minAngle);
    double thetaH = degreeToRadian(ship.maxAngle);

    double sinLow = sin(2.0 * thetaL);
    double sinHigh = sin(2.0 * thetaH);

    double minimumSin = (sinLow < sinHigh) ? sinLow : sinHigh;
    double maximumSin = (ship.minAngle <= 45.0 && ship.maxAngle >= 45.0) ? 1.0 : ((sinLow > sinHigh) ? sinLow : sinHigh);

    *minimumRange = (ship.minVelocity * ship.minVelocity * minimumSin) / G;
    *maximumRange = (ship.maxVelocity * ship.maxVelocity * maximumSin) / G;
}

/* Calculate Battleship Shell Hit Time & Check vertical angle validity */
double calculateHitTime(double distance, double velocity, double minAngleLimit)
{
    if (distance <= 0.0)
        return 0.0;

    double value = (G * distance) / (velocity * velocity);

    if (value > 1.0)
        return -1.0; /* Distance out of maximum trajectory range */

    double angleRad = 0.5 * asin(value);
    double angleDeg = angleRad * 180.0 / PI;

    /* Check lower angle trajectory against gun jamming limit */
    if (angleDeg < minAngleLimit)
    {
        /* Try high angle trajectory (90 - angleDeg) */
        double highAngleDeg = 90.0 - angleDeg;
        if (highAngleDeg < minAngleLimit)
        {
            return -1.0; /* Cannot hit target within allowed angle range */
        }
        angleRad = degreeToRadian(highAngleDeg);
    }

    double time = (2.0 * velocity * sin(angleRad)) / G;
    return time;
}

/* Generate Escort Ships */
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

/* Save step-by-step results for Part 1-B */
void saveStepResult(const char *filename, int simType, int step, Battleship b, EscortShip escorts[], int numEscorts, int hitCount, int sankByIndex)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) return;

    fprintf(file, "PART 1-B SIMULATION %d - ITERATION STEP %d\n", simType, step);
    fprintf(file, "=============================================\n\n");
    fprintf(file, "Battleship Position : (%.2f, %.2f)\n", b.x, b.y);
    fprintf(file, "Battleship Status   : %s\n\n", b.alive ? "ALIVE" : "DESTROYED");

    if (!b.alive && sankByIndex != -1)
    {
        fprintf(file, "Battleship was SANK in this step by Escort E%d (%s)!\n", escorts[sankByIndex].id, escorts[sankByIndex].type);
    }
    else
    {
        fprintf(file, "Escorts Destroyed in this step : %d\n", hitCount);
    }

    fprintf(file, "\nSurviving Escorts Status:\n");
    fprintf(file, "-------------------------\n");
    for (int i = 0; i < numEscorts; i++)
    {
        fprintf(file, "E%d (%s) at (%.2f, %.2f) - Status: %s\n",
                escorts[i].id, escorts[i].type, escorts[i].x, escorts[i].y,
                escorts[i].alive ? "ALIVE" : "DESTROYED");
    }

    fclose(file);
}

/* Helper function to fully reset and copy initial escort states for Simulation 2 */
void resetEscorts(EscortShip dest[], EscortShip src[], int n)
{
    for (int i = 0; i < n; i++)
    {
        dest[i] = src[i];
        dest[i].alive = 1;
    }
}

int main()
{
    double canvasD;
    int numberOfEscorts, kPoints, tJamStep;
    double thetaMinJammed = 0.0;
    unsigned int seed;

    Battleship battleship;
    EscortShip initialEscorts[MAX_ESCORTS];
    EscortShip simEscorts[MAX_ESCORTS];
    Point path[MAX_PATH_POINTS];

    printf("\n=======================================\n");
    printf("     ADVANCED NAVAL SIMULATOR 1-B      \n");
    printf("=======================================\n\n");

    printf("Enter random seed value: ");
    if (scanf("%u", &seed) != 1) return 1;
    srand(seed);

    printf("Enter battlefield canvas size D: ");
    if (scanf("%lf", &canvasD) != 1 || canvasD <= 0) return 1;

    printf("Enter number of Escort ships (1-%d): ", MAX_ESCORTS);
    if (scanf("%d", &numberOfEscorts) != 1 || numberOfEscorts <= 0 || numberOfEscorts > MAX_ESCORTS) return 1;

    printf("\nEnter Battleship type (U/M/R/S): ");
    scanf(" %c", &battleship.type);
    strcpy(battleship.name, "Battleship");

    printf("Enter Battleship max shell velocity VmaxB: ");
    scanf("%lf", &battleship.maxVelocity);

    printf("Enter Battleship initial X position: ");
    scanf("%lf", &battleship.x);
    printf("Enter Battleship initial Y position: ");
    scanf("%lf", &battleship.y);
    battleship.alive = 1;

    printf("Enter number of path points k for Battleship movement (1-%d): ", MAX_PATH_POINTS);
    scanf("%d", &kPoints);

    printf("Enter iteration step t when gun gets jammed (t < k): ");
    scanf("%d", &tJamStep);

    printf("Enter jammed minimum vertical angle theta_min (0 < theta_min < 30): ");
    scanf("%lf", &thetaMinJammed);

    /* Generate k random path points for Battleship */
    for (int i = 0; i < kPoints; i++)
    {
        path[i].x = randomDouble(0.0, canvasD);
        path[i].y = randomDouble(0.0, canvasD);
    }

    /* Generate Escort Ships */
    for (int i = 0; i < numberOfEscorts; i++)
    {
        generateEscort(&initialEscorts[i], i + 1, canvasD, battleship.maxVelocity);
    }

    /* ===================================================
       SAVE INITIAL CONDITIONS TO TEXT FILE (WITH PATH POINTS)
       =================================================== */
    FILE *initFile = fopen("part1b_initial.txt", "w");
    if (initFile != NULL)
    {
        fprintf(initFile, "=== PART 1-B INITIAL CONDITIONS ===\n\n");
        fprintf(initFile, "Canvas Size D: %.2f | Seed: %u | Total Path Points (k): %d\n", canvasD, seed, kPoints);
        fprintf(initFile, "Battleship Type: %c | Max Velocity: %.2f | Initial Position: (%.2f, %.2f)\n\n", 
                battleship.type, battleship.maxVelocity, battleship.x, battleship.y);
        
        fprintf(initFile, "Battleship Path Points:\n");
        fprintf(initFile, "------------------------\n");
        for (int i = 0; i < kPoints; i++)
        {
            fprintf(initFile, "Point %d: (%.2f, %.2f)\n", i + 1, path[i].x, path[i].y);
        }
        fprintf(initFile, "\n");

        fprintf(initFile, "Escort Ships Initial Configuration:\n");
        fprintf(initFile, "-------------------------------------\n");
        for (int i = 0; i < numberOfEscorts; i++)
        {
            fprintf(initFile, "E%d [%s - %s] Pos: (%.2f, %.2f) | MaxV: %.2f | MinV: %.2f | Angles: [%.2f, %.2f]\n",
                    initialEscorts[i].id, initialEscorts[i].type, initialEscorts[i].typeName,
                    initialEscorts[i].x, initialEscorts[i].y, initialEscorts[i].maxVelocity,
                    initialEscorts[i].minVelocity, initialEscorts[i].minAngle, initialEscorts[i].maxAngle);
        }
        fclose(initFile);
    }

    /* Setup Results File for Final Summary */
    FILE *resFile = fopen("part1b_results.txt", "w");
    if (resFile != NULL)
    {
        fprintf(resFile, "=== PART 1-B SIMULATION SUMMARY REPORT ===\n\n");
    }

    int sim1Survivors = 0;
    int sim2Survivors = 0;

    /* ===================================================
       SIMULATION 1: Path Movement (No Gun Jamming)
       =================================================== */
    printf("\n---------------------------------------\n");
    printf(" STARTING SIMULATION 1: PATH MOVEMENT   \n");
    printf("---------------------------------------\n");

    resetEscorts(simEscorts, initialEscorts, numberOfEscorts);
    Battleship bSim1 = battleship;
    int sim1EndedBySinking = 0;

    for (int step = 1; step <= kPoints; step++)
    {
        bSim1.x = path[step - 1].x;
        bSim1.y = path[step - 1].y;

        printf("\n[Sim 1 - Step %d] B moved to (%.2f, %.2f)\n", step, bSim1.x, bSim1.y);

        /* Step A: Check if any living Escort can hit B */
        int sankByIndex = -1;
        for (int i = 0; i < numberOfEscorts; i++)
        {
            if (!simEscorts[i].alive) continue;

            double dist = calculateDistance(simEscorts[i].x, simEscorts[i].y, bSim1.x, bSim1.y);
            double minR, maxR;
            escortAttackRange(simEscorts[i], &minR, &maxR);

            if (dist >= minR && dist <= maxR)
            {
                sankByIndex = i;
                break;
            }
        }

        if (sankByIndex != -1)
        {
            bSim1.alive = 0;
            sim1EndedBySinking = 1;
            printf(" -> BATTLESHIP DESTROYED by Escort E%d (%s) at step %d!\n", simEscorts[sankByIndex].id, simEscorts[sankByIndex].type, step);
            char fileName[60];
            sprintf(fileName, "part1b_sim1_step_%d.txt", step);
            saveStepResult(fileName, 1, step, bSim1, simEscorts, numberOfEscorts, 0, sankByIndex);
            break;
        }

        /* Step B: B counter-attacks surviving Escorts */
        int hitCount = 0;
        double bMaxRange = (bSim1.maxVelocity * bSim1.maxVelocity) / G;

        for (int i = 0; i < numberOfEscorts; i++)
        {
            if (!simEscorts[i].alive) continue;

            double dist = calculateDistance(bSim1.x, bSim1.y, simEscorts[i].x, simEscorts[i].y);
            double hitTime = calculateHitTime(dist, bSim1.maxVelocity, 0.0);

            if (dist <= bMaxRange && hitTime > 0)
            {
                simEscorts[i].alive = 0; 
                hitCount++;
                printf(" -> B HIT E%d (%s)\n", simEscorts[i].id, simEscorts[i].type);
            }
        }

        char fileName[60];
        sprintf(fileName, "part1b_sim1_step_%d.txt", step);
        saveStepResult(fileName, 1, step, bSim1, simEscorts, numberOfEscorts, hitCount, -1);
    }

    for(int i = 0; i < numberOfEscorts; i++) {
        if(simEscorts[i].alive) sim1Survivors++;
    }

    if (resFile != NULL)
    {
        fprintf(resFile, "--- SIMULATION 1 RESULTS ---\n");
        fprintf(resFile, "Battleship Status: %s\n", bSim1.alive ? "SURVIVED" : "DESTROYED");
        fprintf(resFile, "Surviving Escorts Count: %d / %d\n\n", sim1Survivors, numberOfEscorts);
    }

    /* ===================================================
       SIMULATION 2: Gun Jammed after t iterations
       =================================================== */
    printf("\n---------------------------------------\n");
    printf(" STARTING SIMULATION 2: GUN JAMMING     \n");
    printf("---------------------------------------\n");

    resetEscorts(simEscorts, initialEscorts, numberOfEscorts);
    Battleship bSim2 = battleship;
    int sim2EndedBySinking = 0;

    for (int step = 1; step <= kPoints; step++)
    {
        bSim2.x = path[step - 1].x;
        bSim2.y = path[step - 1].y;

        double currentMinAngle = (step >= tJamStep) ? thetaMinJammed : 0.0;
        if (step >= tJamStep)
        {
            printf("\n[Sim 2 - Step %d] B moved to (%.2f, %.2f) [GUN JAMMED: Angle >= %.2f deg]\n", step, bSim2.x, bSim2.y, thetaMinJammed);
        }
        else
        {
            printf("\n[Sim 2 - Step %d] B moved to (%.2f, %.2f)\n", step, bSim2.x, bSim2.y);
        }

        /* Check Escort attacks after jam test starts */
        int sankByIndex = -1;

        if (step >= tJamStep)
{
    for (int i = 0; i < numberOfEscorts; i++)
    {
        if (!simEscorts[i].alive) continue;

        double dist = calculateDistance(simEscorts[i].x, simEscorts[i].y, bSim2.x, bSim2.y);
        double minR, maxR;

        escortAttackRange(simEscorts[i], &minR, &maxR);

        if (dist >= minR && dist <= maxR)
        {
            sankByIndex = i;
            break;
        }
    }
}

        if (sankByIndex != -1)
        {
            bSim2.alive = 0;
            sim2EndedBySinking = 1;
            printf(" -> BATTLESHIP DESTROYED by Escort E%d (%s) at step %d!\n", simEscorts[sankByIndex].id, simEscorts[sankByIndex].type, step);
            char fileName[60];
            sprintf(fileName, "part1b_sim2_step_%d.txt", step);
            saveStepResult(fileName, 2, step, bSim2, simEscorts, numberOfEscorts, 0, sankByIndex);
            break;
        }

        /* Step B: B counter-attacks with angle constraints if jammed */
        int hitCount = 0;
        double bMaxRange = (bSim2.maxVelocity * bSim2.maxVelocity) / G;

        for (int i = 0; i < numberOfEscorts; i++)
        {
            if (!simEscorts[i].alive) continue;

            double dist = calculateDistance(bSim2.x, bSim2.y, simEscorts[i].x, simEscorts[i].y);
            double hitTime = calculateHitTime(dist, bSim2.maxVelocity, currentMinAngle);

            if (dist <= bMaxRange && hitTime > 0)
            {
                simEscorts[i].alive = 0;
                hitCount++;
                printf(" -> B HIT E%d (%s)\n", simEscorts[i].id, simEscorts[i].type);
            }
        }

        char fileName[60];
        sprintf(fileName, "part1b_sim2_step_%d.txt", step);
        saveStepResult(fileName, 2, step, bSim2, simEscorts, numberOfEscorts, hitCount, -1);
    }

    for(int i = 0; i < numberOfEscorts; i++) {
        if(simEscorts[i].alive) sim2Survivors++;
    }

    if (resFile != NULL)
    {
        fprintf(resFile, "--- SIMULATION 2 RESULTS (GUN JAMMED) ---\n");
        fprintf(resFile, "Battleship Status: %s\n", bSim2.alive ? "SURVIVED" : "DESTROYED");
        fprintf(resFile, "Surviving Escorts Count: %d / %d\n", sim2Survivors, numberOfEscorts);
        fclose(resFile);
    }

    printf("\n=======================================\n");
    printf(" SIMULATION COMPLETED! Path points & reset logic added.\n");
    printf("=======================================\n");

    return 0;
}
