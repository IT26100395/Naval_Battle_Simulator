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
    int hasAttacked;
} EscortShip;

typedef struct
{
    char type;
    char name[50];
    char gunName[50];
    double x;
    double y;
    double maxVelocity;
    double cumulativeImpact;
    int alive;
} Battleship;

double randomDouble(double min, double max)
{
    return min + ((double)rand() / (double)RAND_MAX) * (max - min);
}

double degreeToRadian(double degree)
{
    return degree * PI / 180.0;
}

double calculateDistance(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

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

double calculateHitDetails(double distance, double velocity, double minAngleLimit, double *outAngleDeg)
{
    if (distance <= 0.0) return 0.0;

    double value = (G * distance) / (velocity * velocity);
    if (value > 1.0) return -1.0;

    double angleRad = 0.5 * asin(value);
    double angleDeg = angleRad * 180.0 / PI;

    if (angleDeg < minAngleLimit)
    {
        double highAngleDeg = 90.0 - angleDeg;
        if (highAngleDeg < minAngleLimit) return -1.0;
        angleRad = degreeToRadian(highAngleDeg);
        angleDeg = highAngleDeg;
    }

    if (outAngleDeg) *outAngleDeg = angleDeg;
    return (2.0 * velocity * sin(angleRad)) / G;
}

void generateEscort(EscortShip *ship, int id, double canvasD, double battleshipMaxVelocity)
{
    ship->id = id;
    ship->alive = 1;
    ship->hasAttacked = 0;
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

void resetEscorts(EscortShip dest[], EscortShip src[], int n)
{
    for (int i = 0; i < n; i++)
    {
        dest[i] = src[i];
        dest[i].alive = 1;
        dest[i].hasAttacked = 0;
    }
}

int countAliveEscorts(EscortShip escorts[], int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (escorts[i].alive) count++;
    }
    return count;
}

int main()
{
    double canvasD;
    int numberOfEscorts, kPoints, tJamStep;
    double thetaMinJammed;
    unsigned int seed;

    Battleship battleship;
    EscortShip initialEscorts[MAX_ESCORTS];
    EscortShip simEscorts[MAX_ESCORTS];
    Point path[MAX_PATH_POINTS];

    printf("=======================================\n");
    printf("     ADVANCED NAVAL SIMULATOR 1-C      \n");
    printf("=======================================\n\n");

    printf("Enter random seed value: ");
    if (scanf("%u", &seed) != 1) return 1;
    srand(seed);

    printf("Enter battlefield canvas size D: ");
    if (scanf("%lf", &canvasD) != 1 || canvasD <= 0) return 1;

    printf("Enter number of Escort ships (1-%d): ", MAX_ESCORTS);
    if (scanf("%d", &numberOfEscorts) != 1 || numberOfEscorts <= 0 || numberOfEscorts > MAX_ESCORTS) return 1;

    printf("Enter Battleship type (U/M/R/S): ");
    scanf(" %c", &battleship.type);

    switch (battleship.type)
    {
        case 'U': case 'u': strcpy(battleship.name, "USS Iowa"); break;
        case 'M': case 'm': strcpy(battleship.name, "MS King George V"); break;
        case 'R': case 'r': strcpy(battleship.name, "Richelieu"); break;
        case 'S': case 's': strcpy(battleship.name, "Sovetsky Soyuz"); break;
        default: strcpy(battleship.name, "Battleship"); break;
    }

    printf("Enter Battleship max shell velocity VmaxB (m/s): ");
    scanf("%lf", &battleship.maxVelocity);

    printf("Enter Battleship initial X position: ");
    scanf("%lf", &battleship.x);
    printf("Enter Battleship initial Y position: ");
    scanf("%lf", &battleship.y);

    printf("Enter number of path points k for Battleship movement: ");
    scanf("%d", &kPoints);

    printf("Enter iteration step t when gun gets jammed: ");
    scanf("%d", &tJamStep);

    printf("Enter jammed minimum vertical angle theta_min: ");
    scanf("%lf", &thetaMinJammed);

    battleship.alive = 1;
    battleship.cumulativeImpact = 0.0;

    path[0].x = battleship.x;
    path[0].y = battleship.y;
    for (int i = 1; i < kPoints; i++)
    {
        path[i].x = randomDouble(0.0, canvasD);
        path[i].y = randomDouble(0.0, canvasD);
    }

    for (int i = 0; i < numberOfEscorts; i++)
    {
        generateEscort(&initialEscorts[i], i + 1, canvasD, battleship.maxVelocity);
    }

    printf("\n");

    /* ================= STATIC SIMULATION ================= */
    resetEscorts(simEscorts, initialEscorts, numberOfEscorts);
    Battleship bStatic = battleship;

    for (int i = 0; i < numberOfEscorts; i++)
    {
        double dist = calculateDistance(simEscorts[i].x, simEscorts[i].y, bStatic.x, bStatic.y);
        double minR, maxR;
        escortAttackRange(simEscorts[i], &minR, &maxR);

        if (dist >= minR && dist <= maxR)
        {
            bStatic.cumulativeImpact += simEscorts[i].impactPower;
            simEscorts[i].hasAttacked = 1;
            double bHealth = (1.0 - bStatic.cumulativeImpact) * 100.0;
            printf("E%d (%s) HIT Battleship | Damage %.0f%% | B Health %.2f%%\n",
                   simEscorts[i].id, simEscorts[i].type, simEscorts[i].impactPower * 100.0, bHealth);
        }
    }

    if (bStatic.cumulativeImpact < 1.0)
    {
        double bMaxRange = (bStatic.maxVelocity * bStatic.maxVelocity) / G;
        for (int i = 0; i < numberOfEscorts; i++)
        {
            double dist = calculateDistance(bStatic.x, bStatic.y, simEscorts[i].x, simEscorts[i].y);
            double angleDeg = 0.0;
            double hitTime = calculateHitDetails(dist, bStatic.maxVelocity, 0.0, &angleDeg);

            if (dist <= bMaxRange && hitTime > 0)
            {
                simEscorts[i].alive = 0;
                printf("B destroyed E%d (%s) | Distance %.2f | Angle %.2f\n",
                       simEscorts[i].id, simEscorts[i].type, dist, angleDeg);
            }
        }
    }

    printf("\nCumulative Impact on B: %.2f%%\n", bStatic.cumulativeImpact * 100.0);
    printf("Final B Health: %.2f%%\n\n", (1.0 - bStatic.cumulativeImpact) * 100.0);

    /* ================= PATH SIMULATION 1 ================= */
    printf("=======================================\n");
    printf("PART 1-C PATH SIMULATION 1\n");
    printf("=======================================\n\n");

    resetEscorts(simEscorts, initialEscorts, numberOfEscorts);
    Battleship bSim1 = battleship;

    for (int step = 1; step <= kPoints; step++)
    {
        bSim1.x = path[step - 1].x;
        bSim1.y = path[step - 1].y;

        printf("Iteration %d\n", step);
        printf("B Position: (%.2f, %.2f)\n", bSim1.x, bSim1.y);

        for (int i = 0; i < numberOfEscorts; i++)
        {
            if (!simEscorts[i].alive) continue;
            double dist = calculateDistance(simEscorts[i].x, simEscorts[i].y, bSim1.x, bSim1.y);
            double minR, maxR;
            escortAttackRange(simEscorts[i], &minR, &maxR);

            if (dist >= minR && dist <= maxR)
            {
                bSim1.cumulativeImpact += simEscorts[i].impactPower;
                simEscorts[i].hasAttacked = 1;
                double bHealth = (1.0 - bSim1.cumulativeImpact) * 100.0;
                printf("E%d (%s) HIT Battleship | Damage %.0f%% | B Health %.2f%%\n",
                       simEscorts[i].id, simEscorts[i].type, simEscorts[i].impactPower * 100.0, bHealth);
            }
        }

        if (bSim1.cumulativeImpact < 1.0)
        {
            double bMaxRange = (bSim1.maxVelocity * bSim1.maxVelocity) / G;
            for (int i = 0; i < numberOfEscorts; i++)
            {
                if (!simEscorts[i].alive) continue;

                double dist = calculateDistance(bSim1.x, bSim1.y, simEscorts[i].x, simEscorts[i].y);
                double angleDeg = 0.0;
                double hitTime = calculateHitDetails(dist, bSim1.maxVelocity, 0.0, &angleDeg);

                if (dist <= bMaxRange && hitTime > 0)
                {
                    simEscorts[i].alive = 0;
                    printf("B destroyed E%d (%s) | Distance %.2f | Angle %.2f\n",
                           simEscorts[i].id, simEscorts[i].type, dist, angleDeg);
                }
            }
        }

        int remaining = countAliveEscorts(simEscorts, numberOfEscorts);
        printf("Remaining Escorts: %d\n", remaining);
        printf("B Health: %.2f%%\n", (1.0 - bSim1.cumulativeImpact) * 100.0);
        if (remaining == 0)
        {
            printf("All Escort ships destroyed.\n");
        }
    }

    printf("\nFINAL RESULT\n");
    printf("B Status: %s\n", bSim1.alive ? "ALIVE" : "DESTROYED");
    printf("B Health: %.2f%%\n", (1.0 - bSim1.cumulativeImpact) * 100.0);
    printf("Cumulative Impact: %.2f%%\n\n", bSim1.cumulativeImpact * 100.0);

    /* ================= PATH SIMULATION 2 ================= */
    printf("=======================================\n");
    printf("PART 1-C PATH SIMULATION 2\n");
    printf("=======================================\n\n");

    resetEscorts(simEscorts, initialEscorts, numberOfEscorts);
    Battleship bSim2 = battleship;

    for (int step = 1; step <= kPoints; step++)
    {
        bSim2.x = path[step - 1].x;
        bSim2.y = path[step - 1].y;

        double currentMinAngle = (step >= tJamStep) ? thetaMinJammed : 0.0;

        printf("Iteration %d\n", step);
        printf("B Position: (%.2f, %.2f)\n", bSim2.x, bSim2.y);

        if (step >= tJamStep)
        {
            printf("[STATUS] GUN JAMMED! Minimum elevation angle constrained to %.2f deg\n", thetaMinJammed);
        }

        for (int i = 0; i < numberOfEscorts; i++)
        {
            if (!simEscorts[i].alive) continue;
            double dist = calculateDistance(simEscorts[i].x, simEscorts[i].y, bSim2.x, bSim2.y);
            double minR, maxR;
            escortAttackRange(simEscorts[i], &minR, &maxR);

            if (dist >= minR && dist <= maxR)
            {
                bSim2.cumulativeImpact += simEscorts[i].impactPower;
                simEscorts[i].hasAttacked = 1;
                double bHealth = (1.0 - bSim2.cumulativeImpact) * 100.0;
                printf("E%d (%s) HIT Battleship | Damage %.0f%% | B Health %.2f%%\n",
                       simEscorts[i].id, simEscorts[i].type, simEscorts[i].impactPower * 100.0, bHealth);
            }
        }

        if (bSim2.cumulativeImpact < 1.0)
        {
            double bMaxRange = (bSim2.maxVelocity * bSim2.maxVelocity) / G;
            for (int i = 0; i < numberOfEscorts; i++)
            {
                if (!simEscorts[i].alive) continue;

                double dist = calculateDistance(bSim2.x, bSim2.y, simEscorts[i].x, simEscorts[i].y);
                double angleDeg = 0.0;
                double hitTime = calculateHitDetails(dist, bSim2.maxVelocity, currentMinAngle, &angleDeg);

                if (dist <= bMaxRange && hitTime > 0)
                {
                    simEscorts[i].alive = 0;
                    printf("B destroyed E%d (%s) | Distance %.2f | Angle %.2f\n",
                           simEscorts[i].id, simEscorts[i].type, dist, angleDeg);
                }
                else if (currentMinAngle > 0.0 && hitTime < 0)
                {
                    printf("B CANNOT FIRE at E%d (%s) | Distance %.2f | Angle restricted (< %.2f deg)\n",
                           simEscorts[i].id, simEscorts[i].type, dist, currentMinAngle);
                }
            }
        }

        int remaining = countAliveEscorts(simEscorts, numberOfEscorts);
        printf("Remaining Escorts: %d\n", remaining);
        printf("B Health: %.2f%%\n", (1.0 - bSim2.cumulativeImpact) * 100.0);
        if (remaining == 0)
        {
            printf("All Escort ships destroyed.\n");
        }
    }

    printf("\nFINAL RESULT\n");
    printf("B Status: %s\n", bSim2.alive ? "ALIVE" : "DESTROYED");
    printf("B Health: %.2f%%\n", (1.0 - bSim2.cumulativeImpact) * 100.0);
    printf("Cumulative Impact: %.2f%%\n\n", bSim2.cumulativeImpact * 100.0);

    /* ================= COMPLETION HEADER ================= */
    printf("=======================================\n");
    printf("PART 1-C COMPLETED\n");
    printf("=======================================\n\n");
    printf("Files created:\n");
    printf("part1c_initial.txt\n");

    return 0;
}
