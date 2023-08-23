#include <arm_neon.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int dx;
    int dy;
} Vector2D;

Vector2D computeMotionVector(int32_t block1[][3], int32_t image[5][5], int block_width, int block_height, int image_width, int image_height)
{
    Vector2D motionVector = {0, 0};
    int32_t min_MAD = 2147483647; // Largest possible int

    // iterate over possible positions for block2 in the image
    for (int x = 0; x <= image_width - block_width; x++)
    {
        for (int y = 0; y <= image_height - block_height; y++)
        {
            // compute the MAD between 'block1' and 'block' at position (dx, dy) in the 'image'
            int32x4_t sum = vdupq_n_s32(0);

            for (int i = 0; i < block_height; i++)
            {
                for (int j = 0; j < block_width; j += 4)
                {
                    // Load 4 pixels at once
                    int32x4_t block1Pixels = vld1q_s32(&block1[i][j]);
                    int32x4_t imagePixels = vld1q_s32(&image[i + x][j + y]);

                    // Calculate absolute difference and accumulate sum
                    sum = vaddq_s32(sum, vabdq_s32(block1Pixels, imagePixels));
                }
            }

            // Add 4 values stored in 'sum' vector
            int32_t current_MAD = (vgetq_lane_s32(sum, 0) + vgetq_lane_s32(sum, 1) + vgetq_lane_s32(sum, 2) + vgetq_lane_s32(sum, 3)) / (block_width * block_height);

            // if this MAD is lower than our current minimum, update the minimum and the motion vector
            if (current_MAD < min_MAD)
            {
                min_MAD = current_MAD;
                motionVector.dx = x;
                motionVector.dy = y;
            }
        }
    }
    return motionVector;
}

int main()
{
    int32_t block1[3][3] = {{5, 6, 7}, {8, 9, 10}, {11, 12, 13}};
    int32_t image[5][5] = {{1, 2, 3, 4, 5}, {4, 5, 6, 7, 8}, {7, 8, 9, 10, 11}, {34, 56, 78, 45, 56}, {89, 78, 67, 1, 2}};

    Vector2D motionVector = computeMotionVector(block1, image, 3, 3, 5, 5);

    printf("The estimated motion vector is: dx = %d, dy = %d\n", motionVector.dx, motionVector.dy);

    return 0;
}