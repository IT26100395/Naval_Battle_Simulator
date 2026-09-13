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
    double initialImpactPower; // IP0 for Escort
    double currentImpactPower; // IPn for Escort
    double gamma;              // Gamma for Escort
    int fireCount;             // n (Number of gun firings)
    double health;             // Health of Escort (Starts at 1.0)
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
    double firingDelay; 
    double initialImpactPower; // IP0 for Battleship (1.0)
    double currentImpactPower; // IPn for Battleship
    double gamma;              // Gamma for Battleship
    int fireCount;             // n (Number of gun firings)
    double cumulativeImpact;   // Damage received from Escorts
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

/* Calculate Power-Law Degradation: IPn = IP0 * e^(-gamma * n) */
double calculateDegradedImpact(double ip0, double gamma, int n) {
    return ip0 * exp(-gamma * (double)n);
}

/* Escort Attack Range Annulus */
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

/* Projectile Launch details */
double calculateHitDetails(double distance, double velocity, double minAngleLimit, double *outAngleDeg) {
    if (distance <= 0.0) return 0.0;

    double value = (G * distance) / (velocity * velocity);
    if (value > 1.0) return -1.0; 

    double angleRad = 0.5 * asin(value);
    double angleDeg = angleRad * 180.0 / PI;

    if (angleDeg < minAngleLimit) {
        double highAngleDeg = 90.0 - angleDeg;
        if (highAngleDeg < minAngleLimit) return -1.0;
        angleRad = degreeToRadian(highAngleDeg);
        angleDeg = highAngleDeg;
    }

    if (outAngleDeg) *outAngleDeg = angleDeg;
    return (2.0 * velocity * sin(angleRad)) / G;
}

/* Generate Escort Ships with user-defined TE and Gamma */
void generateEscort(EscortShip *ship, int id, int typeChoice, double canvasD, double battleshipMaxVelocity, double teVal, double gammaVal) {
    ship->id = id;
    ship->alive = 1;
    ship->health = 1.0; 
    ship->fireCount = 0;
    ship->lastFireTime = -999; 
    ship->x = randomDouble(0.0, canvasD);
    ship->y = randomDouble(0.0, canvasD);

    ship->firingDelay = teVal;
    ship->gamma = gammaVal;

    double angleRange = 0.0;

    switch (typeChoice) {
        case 0:
            strcpy(ship->type, "EA");
            strcpy(ship->typeName, "1936A-class Destroyer");
            ship->initialImpactPower = 0.08;
            angleRange = 20.0;
            ship->maxVelocity = 1.20 * battleshipMaxVelocity;
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.60 * ship->maxVelocity);
            break;
        case 1:
            strcpy(ship->type, "EB");
            strcpy(ship->typeName, "Gabbiano-class Corvette");
            ship->initialImpactPower = 0.06;
            angleRange = 30.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
        case 2:
            strcpy(ship->type, "EC");
            strcpy(ship->typeName, "Matsu-class Destroyer");
            ship->initialImpactPower = 0.07;
            angleRange = 25.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
        case 3:
            strcpy(ship->type, "ED");
            strcpy(ship->typeName, "F-class Escort Ships");
            ship->initialImpactPower = 0.05;
            angleRange = 50.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
        default:
            strcpy(ship->type, "EE");
            strcpy(ship->typeName, "Japanese Kaibokan");
            ship->initialImpactPower = 0.04;
            angleRange = 70.0;
            ship->maxVelocity = randomDouble(0.55 * battleshipMaxVelocity, 0.95 * battleshipMaxVelocity);
            ship->minVelocity = randomDouble(0.25 * ship->maxVelocity, 0.70 * ship->maxVelocity);
            break;
    }

    ship->currentImpactPower = ship->initialImpactPower;
    ship->minAngle = randomDouble(1.0, 90.0 - angleRange);
    ship->maxAngle = ship->minAngle + angleRange;
}

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
                if (escorts[idx1].currentImpactPower < escorts[idx2].currentImpactPower) {
                    swapNeeded = 1;
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

    double escortTE[5];
    double escortGamma[5];

    printf("====================================================\n");
    printf("   ADVANCED NAVAL BATTLE SIMULATOR - PART 2-C       \n");
    printf("====================================================\n\n");

    printf("Enter random seed: ");
    if (scanf("%u", &seed) != 1) return 1;
    srand(seed);

    printf("Enter battlefield size D: ");
    scanf("%lf", &canvasD);

    printf("Enter number of Escort ships: ");
    scanf("%d", &numberOfEscorts);

    printf("\nU - USS Iowa\nM - King George V\nR - Richelieu\nS - Sovetsky Soyuz\n");
    printf("Enter Battleship type: ");
    scanf(" %c", &b.type);

    switch (b.type) {
        case 'U': case 'u': strcpy(b.name, "USS Iowa (BB-61)"); break;
        case 'M': case 'm': strcpy(b.name, "MS King George V"); break;
        case 'R': case 'r': strcpy(b.name, "Richelieu"); break;
        case 'S': case 's': strcpy(b.name, "Sovetsky Soyuz-class"); break;
        default: strcpy(b.name, "Battleship"); break;
    }

    printf("Enter B maximum shell velocity: ");
    scanf("%lf", &b.maxVelocity);

    printf("Enter B starting X: ");
    scanf("%lf", &b.x);
    printf("Enter B starting Y: ");
    scanf("%lf", &b.y);

    printf("Enter B firing interval TB: ");
    scanf("%lf", &b.firingDelay);

    printf("Enter B gamma: ");
    scanf("%lf", &b.gamma);

    printf("\nEnter EA TE: "); scanf("%lf", &escortTE[0]);
    printf("Enter EB TE: "); scanf("%lf", &escortTE[1]);
    printf("Enter EC TE: "); scanf("%lf", &escortTE[2]);
    printf("Enter ED TE: "); scanf("%lf", &escortTE[3]);
    printf("Enter EE TE: "); scanf("%lf", &escortTE[4]);

    printf("\nEnter EA gamma: "); scanf("%lf", &escortGamma[0]);
    printf("Enter EB gamma: "); scanf("%lf", &escortGamma[1]);
    printf("Enter EC gamma: "); scanf("%lf", &escortGamma[2]);
    printf("Enter ED gamma: "); scanf("%lf", &escortGamma[3]);
    printf("Enter EE gamma: "); scanf("%lf", &escortGamma[4]);

    printf("\nEnter number of path points k: ");
    scanf("%d", &kPoints);

    printf("Enter gun jam iteration t: ");
    scanf("%d", &tJamStep);

    printf("Enter theta minimum after jam (1-29): ");
    scanf("%lf", &thetaMinJammed);

    printf("\nSelect Target Prioritization Strategy (1-3): ");
    scanf("%d", &strategyChoice);

    b.alive = 1;
    b.cumulativeImpact = 0.0;
    b.initialImpactPower = 1.0;
    b.currentImpactPower = 1.0;
    b.fireCount = 0;

    path[0].x = b.x;
    path[0].y = b.y;
    for (int i = 1; i < kPoints; i++) {
        path[i].x = randomDouble(0.0, canvasD);
        path[i].y = randomDouble(0.0, canvasD);
    }

    for (int i = 0; i < numberOfEscorts; i++) {
        int tChoice = i % 5;
        generateEscort(&escorts[i], i + 1, tChoice, canvasD, b.maxVelocity, escortTE[tChoice], escortGamma[tChoice]);
    }

    /* Output Files Setup */
    FILE *resFile = fopen("part2c_results.txt", "w");
    FILE *initFile = fopen("part2c_initial.txt", "w");

    if (initFile) {
        fprintf(initFile, "=== PART 2-C INITIAL CONDITIONS ===\n\n");
        fprintf(initFile, "Battleship: %s\nIP0: %.4f\nGamma: %.4f\nCurrent IP: %.4f\n\n",
                b.name, b.initialImpactPower, b.gamma, b.currentImpactPower);
        
        fprintf(initFile, "Escort Ship Initial Data:\n");
        for (int i = 0; i < numberOfEscorts; i++) {
            fprintf(initFile, "E%d [%s] IP0: %.4f | Gamma: %.4f | TE: %.2f | Current IP: %.4f\n",
                    escorts[i].id, escorts[i].type, escorts[i].initialImpactPower,
                    escorts[i].gamma, escorts[i].firingDelay, escorts[i].currentImpactPower);
        }
        fclose(initFile);
    }

    if (resFile) {
        fprintf(resFile, "=== PART 2-C SIMULATION RESULTS (IMPACT POWER DEGRADATION) ===\n");
        fprintf(resFile, "Battleship: %s | Gamma_B: %.4f | Initial IP: %.2f\n\n", b.name, b.gamma, b.initialImpactPower);
    }

    double currentSimTime = 0.0;

    for (int step = 1; step <= kPoints; step++) {
        if (b.cumulativeImpact >= 1.0) {
            printf("\n[CRITICAL] Battleship has been DESTROYED! Stopping simulation.\n");
            if (resFile) fprintf(resFile, "\n[CRITICAL] Battleship DESTROYED!\n");
            break;
        }

        b.x = path[step - 1].x;
        b.y = path[step - 1].y;

        if (step > 1) {
            updateEscortPositions(escorts, numberOfEscorts, canvasD);
        }

        double currentMinAngle = (step >= tJamStep) ? thetaMinJammed : 0.0;

        printf("\n====================================================\n");
        printf("PART 2-C STATIONARY SIMULATION\n");
        printf("--- Iteration %d / %d (Time: %.2fs) ---\n", step, kPoints, currentSimTime);
        if (resFile) {
            fprintf(resFile, "\n--- Iteration %d / %d (Time: %.2fs) ---\n", step, kPoints, currentSimTime);
        }

        /* 1. Escorts Attack Battleship */
        for (int i = 0; i < numberOfEscorts; i++) {
            if (!escorts[i].alive) continue;

            double dist = calculateDistance(escorts[i].x, escorts[i].y, b.x, b.y);
            double minR, maxR;
            escortAttackRange(escorts[i], &minR, &maxR);

            if (dist >= minR && dist <= maxR && currentSimTime - escorts[i].lastFireTime >= escorts[i].firingDelay) {
                escorts[i].fireCount++;
                escorts[i].currentImpactPower = calculateDegradedImpact(escorts[i].initialImpactPower, escorts[i].gamma, escorts[i].fireCount);
                
                b.cumulativeImpact += escorts[i].currentImpactPower;
                escorts[i].lastFireTime = currentSimTime; 

                double bHealth = (b.cumulativeImpact >= 1.0) ? 0.0 : (1.0 - b.cumulativeImpact) * 100.0;
                printf("Time %.2f : E%d (%s) Shot %d -> B | IP %.4f | B Health %.2f%%\n",
                       currentSimTime, escorts[i].id, escorts[i].type, escorts[i].fireCount, escorts[i].currentImpactPower, bHealth);
                if (resFile) {
                    fprintf(resFile, "Time %.2f : E%d (%s) Shot %d -> B | IP %.4f | B Health %.2f%%\n",
                            currentSimTime, escorts[i].id, escorts[i].type, escorts[i].fireCount, escorts[i].currentImpactPower, bHealth);
                }
            }
        }

        /* 2. Battleship Attacks Escorts */
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

            for (int k = 0; k < validCount; k++) {
                int idx = validTargets[k];
                double dist = calculateDistance(b.x, b.y, escorts[idx].x, escorts[idx].y);
                double angleDeg = 0.0;
                double flightTime = calculateHitDetails(dist, b.maxVelocity, currentMinAngle, &angleDeg);

                if (flightTime > 0) {
                    b.fireCount++;
                    b.currentImpactPower = calculateDegradedImpact(b.initialImpactPower, b.gamma, b.fireCount);
                    
                    escorts[idx].health -= b.currentImpactPower;

                    if (escorts[idx].health <= 0.0) {
                        escorts[idx].health = 0.0;
                        escorts[idx].alive = 0;
                        printf("Time %.2f : B Shot %d -> E%d (%s) | IP %.4f | E Health 0.00%%\n",
                               currentSimTime, b.fireCount, escorts[idx].id, escorts[idx].type, b.currentImpactPower);
                        printf("E%d DESTROYED!\n", escorts[idx].id);
                        if (resFile) {
                            fprintf(resFile, "Time %.2f : B Shot %d -> E%d (%s) | IP %.4f | E Health 0.00%%\nE%d DESTROYED!\n",
                                    currentSimTime, b.fireCount, escorts[idx].id, escorts[idx].type, b.currentImpactPower, escorts[idx].id);
                        }
                    } else {
                        printf("Time %.2f : B Shot %d -> E%d (%s) | IP %.4f | E Health %.2f%%\n",
                               currentSimTime, b.fireCount, escorts[idx].id, escorts[idx].type, b.currentImpactPower, escorts[idx].health * 100.0);
                        if (resFile) {
                            fprintf(resFile, "Time %.2f : B Shot %d -> E%d (%s) | IP %.4f | E Health %.2f%%\n",
                                    currentSimTime, b.fireCount, escorts[idx].id, escorts[idx].type, b.currentImpactPower, escorts[idx].health * 100.0);
                        }
                    }
                    currentSimTime += b.firingDelay;
                }
            }
        }

        currentSimTime += 1.0;
    }

    /* FINAL SUMMARY SECTION */
    printf("\n========== FINAL SUMMARY ==========\n");
    printf("Battleship Current IP: %.4f\n", b.currentImpactPower);
    printf("Total Firings: %d\n", b.fireCount);

    if (resFile) {
        fprintf(resFile, "\n========== FINAL SUMMARY ==========\n");
        fprintf(resFile, "Battleship Current IP: %.4f\n", b.currentImpactPower);
        fprintf(resFile, "Total Firings: %d\n", b.fireCount);
    }

    for (int i = 0; i < numberOfEscorts; i++) {
        printf("E%d Final IP: %.4f | Health: %.2f%%\n",
               escorts[i].id, escorts[i].currentImpactPower, escorts[i].health * 100.0);
        
        if (resFile) {
            fprintf(resFile, "E%d Final IP: %.4f | Health: %.2f%%\n",
                    escorts[i].id, escorts[i].currentImpactPower, escorts[i].health * 100.0);
        }
    }

    if (resFile) fclose(resFile);
    printf("\nSimulation complete! All details successfully saved to files.\n");

    return 0;
}
