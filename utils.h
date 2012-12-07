#ifndef UTILS_H
#define UTILS_H

//#include <stdio.h> 

/*
 * MAX INDEX
 * return i where z[i] is the maximum value
 */
int max_index(double z[], int N) {
	int j = 0;
	int max_j = 0;
	for ( j = 0; j < N; j++ ) {
		if (z[j] > z[max_j])
			max_j = j;
	}
	return max_j;
}

/*
 * MAX VALUE
 * return z[i] where z[i] is the maximum value
 */
double max_value(double z[], int N) {
	int j = 0;
	double max_v = z[0];
	for ( j = 1; j < N; j++ ) {
		if (z[j] > max_v)
			max_v = z[j];
	}
	return max_v;
}

#endif
