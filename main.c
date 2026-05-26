#include<stdio.h>
#include "mem_alloc.h"

int main()
{
    int *arr = (int *)mem_alloc(4);
    int *y = (int *)mem_alloc(4);
    if(arr == NULL)
    {
        printf("Memory allocation failed");
        return 1;
    }
    else
    {
        for(int i = 0; i < 4; i++)
        {
            arr[i] = i;
        }
    }
    for(int i = 0; i < 4; i++)
    {
        printf("%d\n", arr[i]);
    }
    free_mem((void *)arr);
    free_mem((void *)y);
}