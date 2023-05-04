% % cu
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>

#define MAX_LENGTH 10
#define BLOCK_SIZE 16

        __global__ void
        lcs_kernel(int *dev_c, const char *dev_str1, const char *dev_str2, int n, int m)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    if (i >= n || j >= m)
    {
        return;
    }

    int idx = i * (m + 1) + j;
    dev_c[idx + m + 1] = (dev_str1[i] == dev_str2[j]) ? dev_c[idx] + 1 : max(dev_c[idx + 1], dev_c[idx + m + 2]);
}

int lcs_cuda(const char *str1, const char *str2, char *result)
{
    int n = strlen(str1);
    int m = strlen(str2);

    int *c = (int *)malloc((n + 1) * (m + 1) * sizeof(int));
    memset(c, 0, (n + 1) * (m + 1) * sizeof(int));

    char *dev_str1;
    cudaMalloc((void **)&dev_str1, (n + 1) * sizeof(char));
    cudaMemcpy(dev_str1, str1, (n + 1) * sizeof(char), cudaMemcpyHostToDevice);

    char *dev_str2;
    cudaMalloc((void **)&dev_str2, (m + 1) * sizeof(char));
    cudaMemcpy(dev_str2, str2, (m + 1) * sizeof(char), cudaMemcpyHostToDevice);

    int *dev_c;
    cudaMalloc((void **)&dev_c, (n + 1) * (m + 1) * sizeof(int));
    cudaMemcpy(dev_c, c, (n + 1) * (m + 1) * sizeof(int), cudaMemcpyHostToDevice);

    dim3 block_size(BLOCK_SIZE, BLOCK_SIZE);
    dim3 grid_size((n + block_size.x - 1) / block_size.x, (m + block_size.y - 1) / block_size.y);

    lcs_kernel<<<grid_size, block_size>>>(dev_c, dev_str1, dev_str2, n, m);

    cudaMemcpy(c, dev_c, (n + 1) * (m + 1) * sizeof(int), cudaMemcpyDeviceToHost);

    cudaDeviceSynchronize(); // wait for all CUDA calls to complete before continuing

    int len = c[n * (m + 1) + m];
    result[len] = '\0';

    int i = n - 1;
    int j = m - 1;
    int idx = len - 1;
    while (i >= 0 && j >= 0)
    {
        if (str1[i] == str2[j])
        {
            result[idx--] = str1[i];
            i--;
            j--;
        }
        else if (c[i * (m + 1) + j + 1] > c[(i + 1) * (m + 1) + j])
        {
            i--;
        }
        else
        {
            j--;
        }
    }

    free(c);
    cudaFree(dev_str1);
    cudaFree(dev_str2);
    cudaFree(dev_c);

    return len;
}

int main()
{
    const char *str1 = "KARIM";
    const char *str2 = "KARIM";
    char result[MAX_LENGTH];

    int len = lcs_cuda(str1, str2, result);

    printf("LCS of %s and %s is %s (length %d)\n", str1, str2, result, len);

    return 0;
}