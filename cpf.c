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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "debug.h"
#include "network.h"
#include "dpf.h"

// global estimate
double x_est[DIM_STATE];

void run_cpf(int MY_ID) {

	/*
	 * SETUP.
	 */

	int t = 0, i = 0;
	double err[NUM_SIMULATIONS];

	printf("| T=%d;\n", NUM_SIMULATIONS);
	printf("| I=%d;\n", NUM_PARTICLES);
	printf("| N=%d;\n", NUM_SENSORS);
	printf("| MIN_X=%f;\n", MIN_X);
	printf("| MIN_Y=%f;\n", MIN_Y);
	printf("| LIM_X=%f;\n", LIM_X);
	printf("| LIM_Y=%f;\n", LIM_Y);
	printf("| x_est = zeros(T,2);\n");
	printf("| x_pos = zeros(T,2);\n");
	printf("| x = zeros(T,2,I);\n");
	printf("| y = zeros(T,N);\n");
	printf("| s = zeros(N,2);\n");
	printf("| l = zeros(2,2);\n");
	for (i=0; i < NUM_SENSORS; i++) {
		printf("| s(%d,:) ", i+1); vec_print(sensorPositions[i],2);
	}
	printf("| l(1,:) "); vec_print(lightSource[0],2);
	printf("| l(2,:) "); vec_print(lightSource[1],2);

	/*
	 * INIT.
	 */
	dpf_init();							// initialise filter
	dpf_normalize();					// normalise weights
	dpf_estimate(x_est);				// initial estimate     (not necessary?)

	/*
	 * FILTER.
	 */
	for (t=0; t < NUM_SIMULATIONS; t++) {

		printf("| y(%d,:)",t); vec_print(observations[t],NUM_SENSORS);
		for (i=0; i < NUM_PARTICLES; i++) {
			printf("| x(%d,:,%d)",t,i+1); vec_print(x[i],2);
		}

		/*
		 * IMPORTANCE SAMPLING + WEIGHT UPDATE / each step
		 */

		dpf_importance_sampling();

		dpf_weight_update(observations[t]);
			
		dpf_normalize();		// normalise weights

		/*
		 * ESTIMATE (optional)
		 */
		dpf_estimate(x_est);	// estimate

		/*
		 * MEASURE THE ERROR (for simulation tests only)
		 */
		err[t] = vec_dist(x_est,x_true[t]);

		printf("| x_est(%d,:) ", t); vec_print(x_est,2);
		printf("| x_pos(%d,:) ", t); vec_print(x_true[t],2);

		/*
		 * RESAMPLING (optional)
		 */
		dpf_resample(1.0/NUM_PARTICLES);
	}

	double MAE  = vec_sum(err,NUM_SIMULATIONS)/NUM_SIMULATIONS;
	double SDAE = sqrt(vec_var(err,NUM_SIMULATIONS)); 

	/*
	 * PRINT ERROR
	 */
	printf("[id=%d] ERR  = ", MY_ID); vec_print(err,NUM_SIMULATIONS);
	printf("[id=%d] MAE  = %f\n", MY_ID, MAE);
	printf("[id=%d] SDAE = %f\n", MY_ID, SDAE);

}

int main(int argc, char *argv[]) {

	int seed = ((argc >= 2) ? atoi(argv[1]) : 0);

	printf("Running CPF J=%d motes; seed %d.\n", NUM_SENSORS, seed);

	sseed(seed);

	gen_true_trajectory();		// generate a trajectory
	gen_observations();			// generate observations
	//load_observations();		// load observations 		(for real data, see layout_bin.h)

	run_cpf(0);
	return 0;
}
