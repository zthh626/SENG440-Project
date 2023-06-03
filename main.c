// (x, y) is the position of the current block
// (r, s) is the motion vector (the displacement of the current block A relative to the reference block B)

int x = 0, y = 0, r = 0, s = 0;

int main()
{
    int A[16][16], B[16][16], diff1, diff2, sad = 0;
    int i, j;
    for (i = 0; i < 16; i++)
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