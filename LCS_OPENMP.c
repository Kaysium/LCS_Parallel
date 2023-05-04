#include <stdio.h>
#include <string.h>
#include <omp.h>

#define MAXLEN 100

int max(int a, int b)
{
    return (a > b) ? a : b;
}

void lcs(char *X, char *Y, int m, int n)
{
    int L[m + 1][n + 1];
    int i, j;

// Building L[m+1][n+1] in bottom-up fashion
#pragma omp parallel for private(j) shared(L)
    for (i = 0; i <= m; i++)
    {
        for (j = 0; j <= n; j++)
        {
            if (i == 0 || j == 0)
                L[i][j] = 0;
            else if (X[i - 1] == Y[j - 1])
                L[i][j] = L[i - 1][j - 1] + 1;
            else
                L[i][j] = max(L[i - 1][j], L[i][j - 1]);
        }
    }

    int index = L[m][n];
    char lcs[index + 1];
    lcs[index] = '\0'; // Set the terminating character

    i = m;
    j = n;
    while (i > 0 && j > 0)
    {
        if (X[i - 1] == Y[j - 1])
        {
            lcs[index - 1] = X[i - 1];
            i--;
            j--;
            index--;
        }
        else if (L[i - 1][j] > L[i][j - 1])
            i--;
        else
            j--;
    }

    // Print the longest common subsequence and its length
    printf("The longest common subsequence is: %s\n", lcs);
    printf("The length of the longest common subsequence is: %d", strlen(lcs));
}

int main()
{
    char X[MAXLEN], Y[MAXLEN];
    int m, n;

    printf("Enter the first string (length <= %d):\n", MAXLEN);
    scanf("%s", X);

    printf("Enter the second string (length <= %d):\n", MAXLEN);
    scanf("%s", Y);

    m = strlen(X);
    n = strlen(Y);

    lcs(X, Y, m, n);

    return 0;
}