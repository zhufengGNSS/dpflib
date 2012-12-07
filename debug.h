// For doing tests / debugging.

#include <stdio.h>   

void vec_print(double x[], int N) {
	int i;
	printf(" = [");
	for (i = 0; i < N; i++) {
		printf(" %.2f", x[i]);
	}
	printf(" ];\n");
}
