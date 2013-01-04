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

#include <math.h>

#define DBL_MIN 1e-300

// product of v
double vec_prod(double v[], int N) {
	double p = v[0];
	int i;
	for (i = 1; i < N; i++) {
		p *= v[i];
	}
	return p;
}

// sum(v)
double vec_sum(double v[], int N) {
	double p = v[0];
	int i;
	for (i = 1; i < N; i++) {
		p += v[i];
	}
	return p;
}

// add s to each v[i]
void vec_add(double v[], int N, double s) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] += s;
	}
}

void vec_add_vec(double v[], int N, double u[]) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] += u[i];
	}
}

// mean(v)
double vec_mean(double v[], int N) {
	return vec_sum(v,N)/N;
}

// var(v)
double vec_var(double v[], int N) {
	double m = vec_mean(v, N);
	double s = 0.0;
	int i;
	for (i = 0; i < N; i++) {
		s += v[i];
	}
	return pow((s - m),2) / (N - 1);
}

// x[i] = v
void vec_set(double v[], int N, double v_) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] =  v_;
	}
}

// v[i] = u[i]
void vec_set_vec(double v[], int N, double u[]) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] =  u[i];
	}
}

// v[i] - u[i]
void vec_sub_vec(double v[], int N, double u[]) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] -= u[i];
	}
}

// scalar mult
void vec_mult(double v[], int N, double c) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] *= c;
	}
}
// vector mult (e.g. v * u')
double vec_mult_vec(double v[], int N, double u[]) {
	double ans = 0.0;
	int i;
	for (i = 0; i < N; i++) {
		ans += (v[i] * u[i]);
	}
	return ans;
}

// scalar mult
void vec_div(double v[], int N, double c_) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] /= c_;
	}
}

// copy the first N values of u into v
void vec_copy(double v[], double u[], int N) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] =  u[i];
	}
}

// normalize v, using the constant (faster)
void vec_normalize_with_c(double v[], int N, double c) {
	int i;
	//actual normalization
	for (i = 0; i < N; i++) {
		v[i] = v[i]/c;
	}
}

// exp(v)
void vec_exp(double v[], int N) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] = exp(v[i]);
	}
}

// return a 2D normal vector v at angle theta
void vec_set_angle(double v[], double theta) {
	v[0] = cos(theta);
	v[1] = sin(theta);
}

// computes distance (assume 2D i.e. [x,y]) using pythagorus
double vec_dist(double p1[], double p2[]) {
	return sqrt(pow(p1[0] - p2[0],2) + pow(p1[1] - p2[1],2));
}

// v*v
double vec_square(double v[], int N) {
	return vec_mult_vec(v,N,v);
}

/* not needed
void vec_mat_mult_mat(double v[], int N, double u[]) {
	int i;
	for (i = 0; i < N; i++) {
		v[i] *= u[i];
	}
}
*/

// all elements of v are the same?
int vec_equal(int v[], int N) {
	int i;
	for (i = 1; i < N; i++) {
		if (v[i] != v[i-1]) 
			return 0;
	}
	return 1;
}

