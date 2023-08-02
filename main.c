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

#define MAX_SAD_VAL (BLOCK_SIZE * BLOCK_SIZE * MAX_DIFF)
/*
In the following modified code, we load 8 elements at a time into NEON registers using vld1q_s16 and subtract them using vsubq_s16.
We then calculate the absolute difference using vabsq_s16.
The resulting absolute differences are accumulated into the sad vector using vaddq_s16.
Finally, we reduce the 16-bit accumulators to a single 64-bit value using pairwise additions with vpaddlq_s16, vpaddlq_s32, and vgetq_lane_s64.
*/

// Note: BLOCK_SIZE must be 16, or else we'll have to add an inner loop
int sad(int8_t A[BLOCK_SIZE][BLOCK_SIZE], int8_t B[BLOCK_SIZE][BLOCK_SIZE])
{
    int16x8_t diff, sad = vdupq_n_s16(0);
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
    {
        int16x8_t aVec = vmovl_s8(vld1_s8(&A[i][0]));
        int16x8_t bVec = vmovl_s8(vld1_s8(&B[i][0]));
        diff = vsubq_s16(aVec, bVec);
        int16x8_t absDiff = vabsq_s16(diff);
        sad = vaddq_s16(sad, absDiff);
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
void init_block(int x, int y, int8_t image[IMG_W][IMG_H], int8_t block[BLOCK_SIZE][BLOCK_SIZE])
{

    assert((x + BLOCK_SIZE) <= IMG_W);
    assert((y + BLOCK_SIZE) <= IMG_H);

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j += 8)
        {
            int8x16_t row = vld1q_s8(&image[x + i][y + j]);
            vst1q_s8(&block[i][j], row);
        }
    }
}

// This function initializes an image with a rectangle of size shape_w, shape_h at starting point shape_x, shape_y
void initImage(int8_t image[IMG_W][IMG_H], int shape_w, int shape_h, int shape_x, int shape_y) {
    // Assert that the shape is within the image
    assert(shape_x >= 0);
    assert(shape_y >= 0);
    assert((shape_x + shape_w) <= IMG_W);
    assert((shape_y + shape_h) <= IMG_H);

    for(int i = 0; i < IMG_W; i++){
        for(int j = 0; j < IMG_H; j++){

            // Make the shape
            if (i >= shape_x && i < (shape_x + shape_w) && j >= shape_y && j < (shape_y + shape_h)){
                // Make the shape
                image[i][j] = 7;
            }
            else{
                // Make the background
                image[i][j] = 0;
            }
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
    assert(MAX_SAD_VAL < pow(2, 16));

    // Assert that image is at least big enough to contain one block + 2-block radius
    assert(IMG_W >= (BLOCK_SIZE * 3));
    assert(IMG_H >= (BLOCK_SIZE * 3));

    int8_t imageP[IMG_W][IMG_H] = {{0}};
    int8_t imageQ[IMG_W][IMG_H] = {{0}};

    // Initialize imageP with a rectangle of size 4x4 at starting point (16,16)
    initImage(imageP, 4, 4, 16, 16);

    // Initialize imageQ with a rectangle of size 4x4 at starting point (10,10)
    initImage(imageQ, 4, 4, 10, 10);

    
    int8_t A[BLOCK_SIZE][BLOCK_SIZE] = {{0}}, B[BLOCK_SIZE][BLOCK_SIZE] = {{0}};

    int x, y, r, s;

    int motionVectorX = 0;
    int motionVectorY = 0;
    int minSad = MAX_SAD_VAL;
    int sadVal = 0;

    // Iterate through every block A starting position
    for (x = 0; (x + BLOCK_SIZE) <= IMG_W; x += BLOCK_SIZE)
    {
        for (y = 0; (y + BLOCK_SIZE) <= IMG_H; y += BLOCK_SIZE)
        {
            // Initialize block A
            init_block(x, y, imageP, A);

            // Check the SAD(A,B) for every block B in a 2-block radius around block A
            for (r = BLOCK_SIZE * -2; r < (BLOCK_SIZE * 2); r+=STRIDE)
            {
                for (s = BLOCK_SIZE * -2; s < (BLOCK_SIZE * 2); s+=STRIDE)
                {

                    // Skip SAD if loop is out of bounds
                    if ((x + r + BLOCK_SIZE) > IMG_W ||
                        (y + s + BLOCK_SIZE) > IMG_H ||
                        (x + r) < 0 ||
                        (y + s) < 0)
                        continue;
                    
                    init_block(x + r, y + s, imageQ, B);

                    sadVal = sad(A, B);
                    
                    // Update motion vector if new min SAD found, default to 0 motion vector
                    if ((sadVal < minSad) || (sadVal == minSad && r == 0 && s == 0)) {
                        minSad = sadVal;
                        motionVectorX = r;
                        motionVectorY = s;
                    }
                }
            }

            // Print the best motion vector match for block A
            printf("Motion vector: (%d, %d) for BlockA starting at: (%d, %d)\n", motionVectorX, motionVectorY, x, y);

            // Reset values
            motionVectorX = 0;
            motionVectorY = 0;
            minSad = MAX_SAD_VAL;

        }
    }


    return 0;
}