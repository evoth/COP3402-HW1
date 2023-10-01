#include <stdio.h>

void printBits(size_t const size, void const *const ptr)
{
    unsigned char *b = (unsigned char *)ptr;
    unsigned char byte;
    int i, j;

    for (i = size - 1; i >= 0; i--)
    {
        for (j = 7; j >= 0; j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

int main(int argc, char **argv)
{
    int a = ~0;
    int b = 1 << 2;
    long long int product = (long long int)a * (long long int)b;
    int LO = (int)product;
    int HI = product >> 32;
    printBits(sizeof(int), &a);
    printBits(sizeof(int), &b);
    printBits(sizeof(long long int), &product);
    printBits(sizeof(int), &LO);
    printBits(sizeof(int), &HI);
    printf("a: %d, b: %d, prod: %lld, LO: %d, HI: %d\n", a, b, product, LO, HI);
    return 0;
}