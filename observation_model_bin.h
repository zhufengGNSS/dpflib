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
#include "random.h"
#include "layout_bin.h"

// ERROR RATES
#define FNR 0.05	// false negative rate
#define FPR 0.05	// false positive rate

/*
 * MODEL FUNCTION
 * Given the scenario layout (sensor positions etc), calculate the likelihood of a particle 'xp' given a binary 1 observation from sensor j.
 * @param 	xp[NUM_SENSORS]	a single particle
 * @param 	j 				sensor indice
 * @returns	p(y[j] == 1|xp)	the probability of x, if 1 were on
 */
double likelihood(double xp[], int j) {

	// first check if it is in the square (this is faster than checking inside a triangle)
	if (in_square(xp,sensorPositions[j],lightSource[0],lightSource[1])) { 
		// now check if it's in the detection zone (triangle)
		if (in_triangle(xp,sensorPositions[j],lightSource[0],lightSource[1])) {
			return 1.0 - FPR;
		}
		else
			return FNR;
	}
	// p(xp in region, y[1] = 1) is likely only in the case of a false negative
	else {
		return FNR;
	}
}

/*
 * GENERATE OBSERVATIONS
 * Use the observation model to create observations for a simulated path.
 * @updates observations[][]	the observations for each sensor j and each timestep t
 */
void gen_observations() {
	int t, j;
	for (t=0; t<NUM_SIMULATIONS; t++) {
		for(j=0; j<NUM_SENSORS; j++) {
			// the probability of j being on
			double p = likelihood(x_true[t],j);
			//printf("p(x|y[%d])  = %f\n ", j, p); 
			observations[t][j] = (p > randu()) ? 1.0 : 0.0;
		}
	}
}

