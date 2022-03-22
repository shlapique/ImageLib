/*+++++++++++++++++*/
#include "imglib"
#include <ctime>

// Fisher - Yates shuffle
void FisherShuffle()
{
    std::srand(time(NULL));
    for(int i = rows - 1; i >= 1; --i)
    {
        for(int j = cols; j >= 1; --j)
        {
            int n = rand() % (i + 1);
            int m = rand() % (j + 1);
            std::swap(pixel.r[i][j], pixel.r[n][m]);
            std::swap(pixel.g[i][j], pixel.g[n][m]);
            std::swap(pixel.b[i][j], pixel.b[n][m]);
        }
    }
}
