#include <stdio.h>
#include "damage.h"

void applyDamage(float *health, int *destroyed, float impactPower)
{
    float damage;

    damage = (*health) * impactPower;

    *health = *health - damage;

    if(*health <= 0)
    {
        *health = 0;
        *destroyed = 1;

        printf("Ship Destroyed\n");
    }

   // printf("Damage Applied : %.2f\n", damage);
   // printf("Remaining Health : %.2f\n", *health);
}
