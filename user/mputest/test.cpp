#include <stdio.h>
#include <stdlib.h>

int main()
{
	const int k = 300;
	const int s = 10;
	int i;

	printf("allocate %i times, total %i\n", k, k * s);

	for( i = 0; i < k; i++ )
	{
		malloc(s);
	}

	while(1) {}
}