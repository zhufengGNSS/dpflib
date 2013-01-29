 /*
  *   This program is free software: you can redistribute it and/or modify
  *   it under the terms of the GNU General Public License as published by
  *   the Free Software Foundation, either version 3 of the License, or
  *   (at your option) any later version.
  *
  *   This program is distributed in the hope that it will be useful,
  *   but WITHOUT ANY WARRANTY; without even the implied warranty of
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *   GNU General Public License for more details.
  *
  *   You should have received a copy of the GNU General Public License
  *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

#define TAU 6.2832

#include <stdlib.h>
#include <math.h>
#include "vector.h"

// random uniform in [0,1]
double randu() {
	return (double) rand() / (double) RAND_MAX;
	//return unumbers[++unumber_counter % BUF_NUMBERS];
}

// random uniform in [a,b]
double randu_from_range(double a, double b) {
	return a + (randu()*(b-a));
}

// random gaussian
double randn() {

	double u = 2.0 * randu() - 1.0;
	double v = 2.0 * randu() - 1.0;
	double r = u * u + v * v;

	if (r >= 1.0) 
		return randn();

	if (r == 0.0)
		return 0.0;

	return u * sqrt(-2.0 * log(r) / r);
}

void vec_set_randn(double v[], int N) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] = randn();
	}
}

void vec_set_rand(double v[], int N) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] = randu();
	}
}

/*
void vec_add_randn(double v[], int N) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] += randn();
	}
}
*/

// create normal numbers from uniform numbers (unused)
double randg() {
	double x1 = randu();
	double x2 = randu();
	double r1 = sqrt(-2*log(x1)) * cos(TAU*x2);
	//double r1 = sqrt(-2*ln(x1)) * sin(2*math.pi*x2);
	return r1;
}

// random index given a distribution d (which of course sums to 1.0)
int randd(double d[], int N) {
	int i = 0;
	double total = 0.0;
	// pick a uniform real number r in [0,n] 
	double r = randu();
	// then count up along d until we get to r @todo could be a fraction faster
	while (total <= r) {
		total += d[i++];
	}
	return (i-1);
}

