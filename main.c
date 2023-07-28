#include <stdint.h>
#include <stdio.h>

/* Define constants to improve readability */
#define BLOCK_SIZE 3
#define STRIDE 1

#define IMG_W 67
#define IMG_H 13

int sad(int16_t A[BLOCK_SIZE][BLOCK_SIZE], int16_t B[BLOCK_SIZE][BLOCK_SIZE])
{
    int diff, sad = 0;

    int i, j;

    for (i = 0; i < BLOCK_SIZE; i++)
    {
        for (j = 0; j < BLOCK_SIZE; j++)
        {
            diff = A[i][j] - B[i][j];
            if (diff < 0)
                sad -= diff;
            else
                sad += diff;
        }
    }
    return sad;
}

// This function initializes a block as a block of an image at starting point x, y
void init_block(int x, int y, int16_t image[IMG_W][IMG_H], int16_t block[BLOCK_SIZE][BLOCK_SIZE])
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            block[i][j] = image[x + i][y + j];
        }
    }
}

int main()
{
    int16_t imageA[IMG_W][IMG_H] = {
        "                                     /~\\                           "
        "                                    |oo )                          "
        "                                    _\\=/_                          "
        "                    ___        #   /  _  \\                         "
        "                   / ()\\        \\\\//|/.\\|\\\\                       "
        "                 _|_____|_       \\/  \\_/  ||                       "
        "                | | === | |         |\\ /| ||                       "
        "                |_|  O  |_|         \\_ _/  #                       "
        "                 ||  O  ||          | | |                          "
        "                 ||__*__||          | | |                          "
        "                |~ \\___/ ~|         []|[]                          "
        "                /=\\ /=\\ /=\\         | | |                          "
        "________________[_]_[_]_[_]________/_]_[_\\_________________________"};
    int16_t imageB[IMG_W][IMG_H] = {
        "                                     /~\\                           "
        "                                    |oo )                          "
        "                                    _\\=/_                          "
        "                    ___        #   /  _  \\  #                      "
        "                   / ()\\        \\//|/.\\|\\//                       "
        "                 _|_____|_       \\/  \\_/  \\/                       "
        "                | | === | |         |\\ /|                          "
        "                |_|  O  |_|         \\_ _/                          "
        "                 ||  O  ||          | | |                          "
        "                 ||__*__||          | | |                          "
        "              |~ \\___/ ~|         []|[]                            "
        "               /=\\ /=\\ /=\\         | | |                           "
        "________________[_]_[_]_[_]________/_]_[_\\_________________________"};

    int16_t A[BLOCK_SIZE][BLOCK_SIZE] = {{0}}, B[BLOCK_SIZE][BLOCK_SIZE] = {{0}};

    int x, y, r, s;

    int motionVectorX = 0;
    int motionVectorY = 0;
    int minSad, sadVal = 0;

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
                    // TODO:
                    if (sadVal != 0)
                    {
                        printf("x: %d, y: %d, r: %d, s: %d\n", x, y, r, s);
                        break;
                    }
                    printf("SAD: %d\n", sadVal);
                }
            }
        }
    }

    return 0;
}
