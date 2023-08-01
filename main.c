#include <stdint.h>
#include <stdio.h>
#include <arm_neon.h>
#include <assert.h>
#include <math.h>

/* Define constants to improve readability */
#define BLOCK_SIZE 16
#define STRIDE 1

#define IMG_W 64
#define IMG_H 64

#define MAX_DIFF 8
/*
In the following modified code, we load 8 elements at a time into NEON registers using vld1q_s16 and subtract them using vsubq_s16.
We then calculate the absolute difference using vabsq_s16.
The resulting absolute differences are accumulated into the sad vector using vaddq_s16.
Finally, we reduce the 16-bit accumulators to a single 64-bit value using pairwise additions with vpaddlq_s16, vpaddlq_s32, and vgetq_lane_s64.
*/
int sad(int16_t A[BLOCK_SIZE][BLOCK_SIZE], int16_t B[BLOCK_SIZE][BLOCK_SIZE])
{
    int16x8_t diff, sad = vdupq_n_s16(0);
    int i, j;
    for (i = 0; i < BLOCK_SIZE; i++)
    {
        for (j = 0; j < BLOCK_SIZE; j += 8)
        {
            int16x8_t aVec = vld1q_s16(&A[i][j]);
            int16x8_t bVec = vld1q_s16(&B[i][j]);
            diff = vsubq_s16(aVec, bVec);
            int16x8_t absDiff = vabsq_s16(diff);
            sad = vaddq_s16(sad, absDiff);
        }
    }
    int32x4_t sad32 = vpaddlq_s16(sad);
    int64x2_t sad64 = vpaddlq_s32(sad32);
    int64_t sadTotal = vgetq_lane_s64(sad64, 0) + vgetq_lane_s64(sad64, 1);

    return (int)sadTotal;
}

// This function initializes a block as a block of an image at starting point x, y
/*
To improve the init_block function to use Neon intrinsics, we can take advantage of Neon's 128-bit registers and load and store multiple elements at once.
Here's an updated version of the init_block function using Neon intrinsics:

Explanation:

We use int16x8_t to represent an 8-element vector of 16-bit integers, which corresponds to a Neon register.
The inner loop is executed BLOCK_SIZE times to load and store elements column-wise.
The vld1q_s16 function is used to load a Neon register with 8 consecutive elements from the image array starting at the specified address (&image[x + i][y + j]).
The vst1q_s16 function is used to store the 8 elements from the Neon register to the block array starting at the specified address (&block[i][j]).
By using Neon intrinsics, this updated init_block function can load and store multiple elements at once, effectively improving the memory access efficiency and potentially enhancing the performance of the code.
*/
void init_block(int x, int y, int16_t image[IMG_W][IMG_H], int16_t block[BLOCK_SIZE][BLOCK_SIZE])
{
    for (int i = 0; i < BLOCK_SIZE; i += 8)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            int16x8_t row = vld1q_s16(&image[x + i][y + j]);
            vst1q_s16(&block[i][j], row);
        }
    }
}

int main()
{
    // Assert that block size is compatible with image dimensions
    assert(IMG_W % BLOCK_SIZE == 0);
    assert(IMG_H % BLOCK_SIZE == 0);

    // Assert that block size is compatible with vector length
    assert(BLOCK_SIZE >= 8);
    assert(BLOCK_SIZE % 8 == 0);

    // Assert that max value of sad can be stored in uint16_t
    assert((BLOCK_SIZE * BLOCK_SIZE * MAX_DIFF) < pow(2, 16));

    // Assert that image is at least big enough to contain one block + 2-block radius
    assert(IMG_W >= (BLOCK_SIZE * 3));
    assert(IMG_H >= (BLOCK_SIZE * 3));

    int16_t imageA[IMG_W][IMG_H] = {{0}};
    int16_t imageB[IMG_W][IMG_H] = {{0}};

    for (int i = 0; i < IMG_W; i++)
    {
        for (int j = 0; j < IMG_H; j++)
        {
            imageB[i][j] = 1;
        }
    }

    int16_t A[BLOCK_SIZE][BLOCK_SIZE] = {{0}}, B[BLOCK_SIZE][BLOCK_SIZE] = {{0}};

    int x, y, r, s;

    int motionVectorX = 0;
    int motionVectorY = 0;
    int minSad = 0;
    int sadVal = 0;

    // Iterate through every block A starting position
    for (x = 0; x < (IMG_W - BLOCK_SIZE); x += BLOCK_SIZE)
    {
        for (y = 0; y < (IMG_H - BLOCK_SIZE); y += BLOCK_SIZE)
        {
            // Check the SAD(A,B) for every block B in a 2-block radius
            for (r = 0; r < (BLOCK_SIZE * 2); r++)
            {
                for (s = 0; s < (BLOCK_SIZE * 2); s++)
                {
                    init_block(x, y, imageA, A);
                    init_block(x + r, y + s, imageB, B);

                    sadVal = sad(A, B);
                    
                    // Update motion vector if new min SAD found
                    if (sadVal < minSad) {
                        minSad = sadVal;
                        motionVectorX = r;
                        motionVectorY = s;
                    }

                    if (sadVal != 0)
                    {
                        printf("x: %d, y: %d, r: %d, s: %d\n", x, y, r, s);
                        break;
                    }
                }
            }
        }
    }

    // Print the best motion vector match
    printf("Motion vector: (%d, %d)\n", motionVectorX, motionVectorY);

    return 0;
}