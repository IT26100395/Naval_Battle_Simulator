#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 9.81
#define PI 3.14159265358979323846
#define MAX_ESCORTS 100
#define MAX_PATH_POINTS 50

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
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
    double firingDelay;  
    double lastFireTime; 
} EscortShip;

typedef struct {
    char type;
    char name[50];
    char gunName[50];
    double x;
    double y;
    double maxVelocity;
    double firingDelay; // T_B^q
    double cumulativeImpact;
    int alive;
} Battleship;

/* Helper Functions */
double randomDouble(double min, double max) {
    return min + ((double)rand() / (double)RAND_MAX) * (max - min);
}

double degreeToRadian(double degree) {
    return degree * PI / 180.0;
}

double calculateDistance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

/* Calculate Escort Ship Attack Range Annulus */
void escortAttackRange(EscortShip ship, double *minimumRange, double *maximumRange) {
    double thetaL = degreeToRadian(ship.minAngle);
    double thetaH = degreeToRadian(ship.maxAngle);

    double sinLow = sin(2.0 * thetaL);
    double sinHigh = sin(2.0 * thetaH);

    double minimumSin = (sinLow < sinHigh) ? sinLow : sinHigh;
    double maximumSin = (ship.minAngle <= 45.0 && ship.maxAngle >= 45.0) ? 1.0 : ((sinLow > sinHigh) ? sinLow : sinHigh);

    *minimumRange = (ship.minVelocity * ship.minVelocity * minimumSin) / G;
    *maximumRange = (ship.maxVelocity * ship.maxVelocity * maximumSin) / G;
}

/* Calculate projectile launch angle and time of flight */
double calculateHitDetails(double distance, double velocity, double minAngleLimit, double *outAngleDeg) {
    if (distance <= 0.0) return 0.0;

    double value = (G * distance) / (velocity * velocity);
    if (value > 1.0) return -1.0; // Target out of physical range

    double angleRad = 0.5 * asin(value);
    double angleDeg = angleRad * 180.0 / PI;

    /* Handle Jamming Angle Constraints */
    if (angleDeg < minAngleLimit) {
        double highAngleDeg = 90.0 - angleDeg;
        if (highAngleDeg < minAngleLimit) return -1.0;
        angleRad = degreeToRadian(highAngleDeg);
        angleDeg = highAngleDeg;
    }

    if (outAngleDeg) *outAngleDeg = angleDeg;
    return (2.0 * velocity * sin(angleRad)) / G;
}

/* Generate Escort Ships randomly based on Specification */
void generateEscort(EscortShip *ship, int id, double canvasD, double battleshipMaxVelocity) {
    ship->id = id;
    ship->alive = 1;
    ship->lastFireTime = -999; 
    ship->x = randomDouble(0.0, canvasD);
    ship->y = randomDouble(0.0, canvasD);

    int type = rand() % 5;
    double angleRange = 0.0;

    switch (type) {
        case 0:
            strcpy(ship->type, "EA");
            strcpy(ship->typeName, "1936A-class Destroyer");
            strcpy(ship->gunName, "SK C/34 naval gun");
            ship->impactPower = 0.08;
            ship->firingDelay = 3.0; 
            angleRange = 20.0;
            ship->maxVelocity = 1.20 * battleshipMaxVelocity;
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.60 * ship->maxVelocity);
            break;
        case 1:
            strcpy(ship->type, "EB");
            strcpy(ship->typeName, "Gabbiano-class Corvette");
            strcpy(ship->gunName, "L/47 dual-purpose gun");
            ship->impactPower = 0.06;
            ship->firingDelay = 5.0; 
            angleRange = 30.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
        case 2:
            strcpy(ship->type, "EC");
            strcpy(ship->typeName, "Matsu-class Destroyer");
            strcpy(ship->gunName, "Type 89 dual-purpose gun");
            ship->impactPower = 0.07;
            ship->firingDelay = 4.0; 
            angleRange = 25.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
        case 3:
            strcpy(ship->type, "ED");
            strcpy(ship->typeName, "F-class Escort Ships");
            strcpy(ship->gunName, "SK C/32 naval gun");
            ship->impactPower = 0.05;
            ship->firingDelay = 6.0; 
            angleRange = 50.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
        default:
            strcpy(ship->type, "EE");
            strcpy(ship->typeName, "Japanese Kaibokan");
            strcpy(ship->gunName, "(4.7 inch) naval guns");
            ship->impactPower = 0.04;
            ship->firingDelay = 2.0; 
            angleRange = 70.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
    }

    ship->minAngle = randomDouble(1.0, 90.0 - angleRange);
    ship->maxAngle = ship->minAngle + angleRange;
}

/* Update positions of surviving Escort Ships */
void updateEscortPositions(EscortShip escorts[], int count, double canvasD) {
    for (int i = 0; i < count; i++) {
        if (!escorts[i].alive) continue;
        
        double moveDist = randomDouble(escorts[i].minVelocity, escorts[i].maxVelocity);
        double angle = randomDouble(0.0, 2.0 * PI);

        escorts[i].x += moveDist * cos(angle);
        escorts[i].y += moveDist * sin(angle);

        if (escorts[i].x < 0) escorts[i].x = 0;
        if (escorts[i].x > canvasD) escorts[i].x = canvasD;
        if (escorts[i].y < 0) escorts[i].y = 0;
        if (escorts[i].y > canvasD) escorts[i].y = canvasD;
    }
}

/* Strategy Selector */
void sortTargetsByStrategy(int targetIndices[], int count, EscortShip escorts[], Battleship b, int strategyChoice) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int idx1 = targetIndices[j];
            int idx2 = targetIndices[j + 1];
            int swapNeeded = 0;

            if (strategyChoice == 1) { 
                double d1 = calculateDistance(b.x, b.y, escorts[idx1].x, escorts[idx1].y);
                double d2 = calculateDistance(b.x, b.y, escorts[idx2].x, escorts[idx2].y);
                if (d1 > d2) swapNeeded = 1;
            } 
            else if (strategyChoice == 2) { 
                if (escorts[idx1].impactPower < escorts[idx2].impactPower) {
                    swapNeeded = 1;
                } else if (escorts[idx1].impactPower == escorts[idx2].impactPower) {
                    double d1 = calculateDistance(b.x, b.y, escorts[idx1].x, escorts[idx1].y);
                    double d2 = calculateDistance(b.x, b.y, escorts[idx2].x, escorts[idx2].y);
                    if (d1 > d2) swapNeeded = 1;
                }
            } 
            else if (strategyChoice == 3) { 
                if (escorts[idx1].maxVelocity > escorts[idx2].maxVelocity) {
                    swapNeeded = 1;
                }
            }

            if (swapNeeded) {
                int temp = targetIndices[j];
                targetIndices[j] = targetIndices[j + 1];
                targetIndices[j + 1] = temp;
            }
        }
    }
}

int main() {
    double canvasD;
    int numberOfEscorts, kPoints, tJamStep, strategyChoice;
    double thetaMinJammed;
    unsigned int seed;

    Battleship b;
    EscortShip escorts[MAX_ESCORTS];
    Point path[MAX_PATH_POINTS];

    printf("====================================================\n");
    printf("     NAVAL BATTLE SIMULATOR - CONTINUOUS TIME TICK  \n");
    printf("====================================================\n\n");

    printf("Enter random seed value: ");
    if (scanf("%u", &seed) != 1) return 1;
    srand(seed);

    printf("Enter battlefield canvas size D: ");
    scanf("%lf", &canvasD);

    printf("Enter number of Escort ships (1-%d): ", MAX_ESCORTS);
    scanf("%d", &numberOfEscorts);

    printf("Enter Battleship type (U/M/R/S): ");
    scanf(" %c", &b.type);

    switch (b.type) {
        case 'U': case 'u': 
            strcpy(b.name, "USS Iowa (BB-61)"); 
            strcpy(b.gunName, "50-caliber Mark 7 gun"); 
            break;
        case 'M': case 'm': 
            strcpy(b.name, "MS King George V"); 
            strcpy(b.gunName, "(356 mm) Mark VII gun"); 
            break;
        case 'R': case 'r': 
            strcpy(b.name, "Richelieu"); 
            strcpy(b.gunName, "(15 inch) Mle 1935 gun"); 
            break;
        case 'S': case 's': 
            strcpy(b.name, "Sovetsky Soyuz-class"); 
            strcpy(b.gunName, "(16 inch) B-37 gun"); 
            break;
        default: 
            strcpy(b.name, "Battleship"); 
            strcpy(b.gunName, "Main Gun"); 
            break;
    }

    printf("Enter Battleship max shell velocity VmaxB (m/s): ");
    scanf("%lf", &b.maxVelocity);

    printf("Enter time between Battleship gun firings T_B (seconds): ");
    scanf("%lf", &b.firingDelay);

    printf("Enter Battleship initial X position: ");
    scanf("%lf", &b.x);
    printf("Enter Battleship initial Y position: ");
    scanf("%lf", &b.y);

    printf("Enter number of path points k for Battleship movement: ");
    scanf("%d", &kPoints);

    printf("Enter iteration step t when gun gets jammed: ");
    scanf("%d", &tJamStep);

    printf("Enter jammed minimum vertical angle theta_min (0 < theta < 30): ");
    scanf("%lf", &thetaMinJammed);

    printf("\nSelect Target Prioritization Strategy:\n");
    printf(" 1. Strategy 1: Distance / Closest Target First\n");
    printf(" 2. Strategy 2: Highest Impact Power First\n");
    printf(" 3. Strategy 3: Lowest Escort Speed First\n");
    printf("Choice (1-3): ");
    scanf("%d", &strategyChoice);

    b.alive = 1;
    b.cumulativeImpact = 0.0;

    /* Generate Battleship Path Points */
    path[0].x = b.x;
    path[0].y = b.y;
    for (int i = 1; i < kPoints; i++) {
        path[i].x = randomDouble(0.0, canvasD);
        path[i].y = randomDouble(0.0, canvasD);
    }

    /* Generate Escort Ships */
    for (int i = 0; i < numberOfEscorts; i++) {
        generateEscort(&escorts[i], i + 1, canvasD, b.maxVelocity);
    }

    /* Save Initial Conditions with Updated File Format */
    FILE *initFile = fopen("part2b_initial.txt", "w");
    if (initFile) {
        fprintf(initFile, "--- INITIAL BATTLEFIELD CONDITIONS ---\n");
        fprintf(initFile, "Canvas Size D: %.2f\n", canvasD);
        fprintf(initFile, "Battleship: %s | Type: %c | Gun: %s\n", b.name, b.type, b.gunName);
        fprintf(initFile, "B Initial Pos: (%.2f, %.2f) | VmaxB: %.2f m/s | Firing Delay T_B: %.2f s\n", b.x, b.y, b.maxVelocity, b.firingDelay);
        fprintf(initFile, "Strategy Chosen: Strategy %d\n\n", strategyChoice);
        fprintf(initFile, "Escort Ships Initial Data:\n");
        for (int i = 0; i < numberOfEscorts; i++) {
            fprintf(initFile, "E%d [%s] Pos: (%.2f, %.2f) | Vmin: %.2f | Vmax: %.2f | MinAngle: %.2f | MaxAngle: %.2f | Impact: %.2f | Fire Delay: %.2f\n",
                    escorts[i].id, escorts[i].type, escorts[i].x, escorts[i].y,
                    escorts[i].minVelocity, escorts[i].maxVelocity, escorts[i].minAngle, escorts[i].maxAngle, escorts[i].impactPower, escorts[i].firingDelay);
        }
        fclose(initFile);
    }

    FILE *resFile = fopen("part2b_results.txt", "w");

    printf("\n====================================================\n");
    printf("              STARTING SIMULATION                   \n");
    printf("====================================================\n\n");
    if (resFile) fprintf(resFile, "=== SIMULATION RESULTS ===\n\n");

    /* Global Continuous Time Clock */
    double currentSimTime = 0.0;

    for (int step = 1; step <= kPoints; step++) {
        if (b.cumulativeImpact >= 1.0) {
            printf("[CRITICAL] Battleship has been DESTROYED! Stopping simulation.\n");
            if (resFile) fprintf(resFile, "[CRITICAL] Battleship has been DESTROYED! Stopping simulation.\n");
            break;
        }

        b.x = path[step - 1].x;
        b.y = path[step - 1].y;

        if (step > 1) {
            updateEscortPositions(escorts, numberOfEscorts, canvasD);
        }

        double currentMinAngle = (step >= tJamStep) ? thetaMinJammed : 0.0;

        printf("\n--- Iteration %d / %d (Current Time: %.2fs) ---\n", step, kPoints, currentSimTime);
        printf("Battleship Position: (%.2f, %.2f)\n", b.x, b.y);
        if (resFile) {
            fprintf(resFile, "\n--- Iteration %d / %d (Current Time: %.2fs) ---\n", step, kPoints, currentSimTime);
            fprintf(resFile, "Battleship Position: (%.2f, %.2f)\n", b.x, b.y);
        }

        if (step >= tJamStep) {
            printf("[GUN STATUS] JAMMED! Elevation restricted to theta >= %.2f deg\n", thetaMinJammed);
            if (resFile) fprintf(resFile, "[GUN STATUS] JAMMED! Elevation restricted to theta >= %.2f deg\n", thetaMinJammed);
        }

        /* 1. Escorts Attack Battleship with Firing Delay Condition */
        for (int i = 0; i < numberOfEscorts; i++) {
            if (!escorts[i].alive) continue;

            double dist = calculateDistance(escorts[i].x, escorts[i].y, b.x, b.y);
            double minR, maxR;
            escortAttackRange(escorts[i], &minR, &maxR);

            if (dist >= minR && dist <= maxR && currentSimTime - escorts[i].lastFireTime >= escorts[i].firingDelay) {
                b.cumulativeImpact += escorts[i].impactPower;
                escorts[i].lastFireTime = currentSimTime; 

                double bHealth = (b.cumulativeImpact >= 1.0) ? 0.0 : (1.0 - b.cumulativeImpact) * 100.0;
                printf("[ATTACK] E%d (%s) HITS Battleship! Damage: %.0f%% | B Health: %.2f%%\n",
                       escorts[i].id, escorts[i].type, escorts[i].impactPower * 100.0, bHealth);
                if (resFile) {
                    fprintf(resFile, "[ATTACK] E%d (%s) HITS Battleship! Damage: %.0f%% | B Health: %.2f%%\n",
                            escorts[i].id, escorts[i].type, escorts[i].impactPower * 100.0, bHealth);
                }
            }
        }

        /* 2. Battleship Counter-Attacks */
        if (b.cumulativeImpact < 1.0) {
            double bMaxRange = (b.maxVelocity * b.maxVelocity) / G;
            int validTargets[MAX_ESCORTS];
            int validCount = 0;

            for (int i = 0; i < numberOfEscorts; i++) {
                if (!escorts[i].alive) continue;
                double dist = calculateDistance(b.x, b.y, escorts[i].x, escorts[i].y);
                if (dist <= bMaxRange) {
                    validTargets[validCount++] = i;
                }
            }

            sortTargetsByStrategy(validTargets, validCount, escorts, b, strategyChoice);

            printf("\n--> Battleship Attack Order (Strategy %d):\n[ ", strategyChoice);
            if (resFile) fprintf(resFile, "\n--> Battleship Attack Order (Strategy %d):\n[ ", strategyChoice);
            for (int k = 0; k < validCount; k++) {
                printf("E%d(%s, IP:%.2f) ", escorts[validTargets[k]].id, escorts[validTargets[k]].type, escorts[validTargets[k]].impactPower);
                if (resFile) fprintf(resFile, "E%d(%s, IP:%.2f) ", escorts[validTargets[k]].id, escorts[validTargets[k]].type, escorts[validTargets[k]].impactPower);
            }
            printf("]\n");
            if (resFile) fprintf(resFile, "]\n");

            for (int k = 0; k < validCount; k++) {
                int idx = validTargets[k];
                double dist = calculateDistance(b.x, b.y, escorts[idx].x, escorts[idx].y);
                double angleDeg = 0.0;
                double flightTime = calculateHitDetails(dist, b.maxVelocity, currentMinAngle, &angleDeg);

                if (flightTime > 0) {
                    escorts[idx].alive = 0;
                    printf("Time %6.2fs: B FIRED & DESTROYED E%d (%s) | Dist: %.2fm | Launch Angle: %.2f deg | Flight Time: %.2fs\n",
                           currentSimTime, escorts[idx].id, escorts[idx].type, dist, angleDeg, flightTime);
                    if (resFile) {
                        fprintf(resFile, "Time %6.2fs: B FIRED & DESTROYED E%d (%s) | Dist: %.2fm | Launch Angle: %.2f deg | Flight Time: %.2fs\n",
                                currentSimTime, escorts[idx].id, escorts[idx].type, dist, angleDeg, flightTime);
                    }
                    currentSimTime += b.firingDelay;
                } else if (currentMinAngle > 0.0 && flightTime < 0) {
                    printf("Time %6.2fs: B CANNOT FIRE at E%d (%s) | Dist: %.2fm | Angle restricted below %.2f deg\n",
                           currentSimTime, escorts[idx].id, escorts[idx].type, dist, currentMinAngle);
                    if (resFile) {
                        fprintf(resFile, "Time %6.2fs: B CANNOT FIRE at E%d (%s) | Dist: %.2fm | Angle restricted below %.2f deg\n",
                                currentSimTime, escorts[idx].id, escorts[idx].type, dist, currentMinAngle);
                    }
                }
            }
        }

        /* Advance simulation clock by 1.0 second for the movement step tick */
        currentSimTime += 1.0;
    }

    /* Print Final Summary */
    printf("\n====================================================\n");
    printf("                  FINAL SUMMARY                     \n");
    printf("====================================================\n");
    printf("Battleship Status : %s\n", (b.cumulativeImpact < 1.0) ? "SURVIVED" : "DESTROYED");
    printf("Cumulative Impact : %.2f%%\n", b.cumulativeImpact * 100.0);
    printf("Battleship Health : %.2f%%\n", (b.cumulativeImpact >= 1.0) ? 0.0 : (1.0 - b.cumulativeImpact) * 100.0);

    if (resFile) {
        fprintf(resFile, "\n====================================================\n");
        fprintf(resFile, "                  FINAL SUMMARY                     \n");
        fprintf(resFile, "====================================================\n");
        fprintf(resFile, "Battleship Status : %s\n", (b.cumulativeImpact < 1.0) ? "SURVIVED" : "DESTROYED");
        fprintf(resFile, "Cumulative Impact : %.2f%%\n", b.cumulativeImpact * 100.0);
        fprintf(resFile, "Battleship Health : %.2f%%\n", (b.cumulativeImpact >= 1.0) ? 0.0 : (1.0 - b.cumulativeImpact) * 100.0);
        fclose(resFile);
    }

    return 0;
}
