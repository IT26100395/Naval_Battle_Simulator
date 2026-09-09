#include "strategy.h"


int selectTarget(float distance[], int size)
{
    int i;

    int target = 0;

    float minimum = distance[0];


    for(i = 1; i < size; i++)
    {
        if(distance[i] < minimum)
        {
            minimum = distance[i];
            target = i;
        }
    }


    return target;
}
