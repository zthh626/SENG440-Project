#include <stdint.h>
#include <stdio.h>

/* Define constants to improve readability */
#define BLOCK_SIZE 16
#define STRIDE 2

int x = 1, y = 2, r = 3, s = 4;

int main()
{
   int A[16][16] = {{0}}, B[16][16] = {{0}}, diff1, diff2, sad = 0;
   int i, j;
   for (i = 0; i < 16; i++){
       for (j = 0; j < 16; j += 2)
       {
           diff1 = A[x + i][y + j] - B[(x + r) + i][(y + s) + j];
           diff2 = A[x + i][y + j + 1] - B[(x + r) + i][(y + s) + j + 1];
           if (diff1 < 0)
               sad -= diff1;
           else
               sad += diff1;
           if (diff2 < 0)
               sad -= diff2;
           else
               sad += diff2;
       }
    }

    printf("SAD: %d\n", sad);
}
