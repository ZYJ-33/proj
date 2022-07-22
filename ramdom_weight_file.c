#include "./config.h"
#include<stdlib.h>
#include <stdio.h>

void main()
{
    FILE* weight_file = fopen("weight_file", "w");
    char random_weight[MAX_WEIGHT_FILE_LEN];
    for(int i=0; i<MAX_WEIGHT_FILE_LEN; i++)
        random_weight[i] = (char)rand();

    fwrite(random_weight, 1, MAX_WEIGHT_FILE_LEN, weight_file);
}

