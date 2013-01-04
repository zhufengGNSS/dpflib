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
