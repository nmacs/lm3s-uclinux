#include <stddef.h>

int crypto_memcmp(const void *in_a, const void *in_b, size_t len)
{
       size_t i;
       const unsigned char *a = in_a;
       const unsigned char *b = in_b;
       unsigned char x = 0;

       for (i = 0; i < len; i++)
               x |= a[i] ^ b[i];

       return x;
}