#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MAX_STRING_LENGTH 1000

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int lcs(char *X, char *Y, int m, int n)
{
    int L[m + 1][n + 1];
    int i, j;

    for (i = 0; i <= m; i++)
    {
        for (j = 0; j <= n; j++)
        {
            if (i == 0 || j == 0)
            {
                L[i][j] = 0;
            }
            else if (X[i - 1] == Y[j - 1])
            {
                L[i][j] = L[i - 1][j - 1] + 1;
            }
            else
            {
                L[i][j] = max(L[i - 1][j], L[i][j - 1]);
            }
        }
    }

    // Add a barrier to synchronize all processes after they finish computing their portion of the L matrix
    MPI_Barrier(MPI_COMM_WORLD);

    return L[m][n];
}

int main(int argc, char **argv)
{
    int my_rank, comm_sz;
    char X[MAX_STRING_LENGTH], Y[MAX_STRING_LENGTH];
    int m, n, local_m, local_n, local_LCS_length, LCS_length = 0;
    int i, j, k;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0)
    {
        printf("Enter the first string (length <= %d):\n", MAX_STRING_LENGTH);
        scanf("%s", X);

        printf("Enter the second string (length <= %d):\n", MAX_STRING_LENGTH);
        scanf("%s", Y);

        m = strlen(X);
        n = strlen(Y);
    }

    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    local_m = m / comm_sz;
    local_n = n / comm_sz;

    char local_X[local_m], local_Y[local_n];

    MPI_Scatter(X, local_m, MPI_CHAR, local_X, local_m, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Scatter(Y, local_n, MPI_CHAR, local_Y, local_n, MPI_CHAR, 0, MPI_COMM_WORLD);

    local_LCS_length = lcs(local_X, local_Y, local_m, local_n);

    MPI_Reduce(&local_LCS_length, &LCS_length, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        char LCS[LCS_length + 1];
        LCS[LCS_length] = '\0';

        i = m;
        j = n;
        k = LCS_length;

        while (i > 0 && j > 0)
        {
            if (X[i - 1] == Y[j - 1])
            {
                LCS[k - 1] = X[i - 1];
                i--;
                j--;
                k--;
            }
            else if (lcs(X, Y, i - 1, j) > lcs(X, Y, i, j - 1))
            {
                i--;
            }
            else
            {
                j--;
            }
        }

        printf("The longest common subsequence is: %s\n", LCS);
        printf("The length of the longest common subsequence is: %d", strlen(LCS));
    }
}