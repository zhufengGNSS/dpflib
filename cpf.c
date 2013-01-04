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

	printf("[CPF] START mote j=%d. J=%d.\n", MY_ID, NUM_SENSORS);

	/*
	 * SETUP.
	 */

	int t = 0, i = 0;
	double err[NUM_SIMULATIONS];

	printf("| x_est = zeros(%d,2);\n", NUM_SIMULATIONS-1);
	printf("| x_pos = zeros(%d,2);\n", NUM_SIMULATIONS-1);
	printf("| x = zeros(%d,%d,%d);\n", NUM_SIMULATIONS-1, 2, NUM_PARTICLES);
	printf("| y = zeros(%d,%d);\n", NUM_SIMULATIONS-1, NUM_SENSORS);
	printf("| s = zeros(%d,%d);\n", NUM_SENSORS, 2);
	printf("| l = zeros(%d,%d);\n", 2, 2);
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
	dpf_estimate(x_est);				// initial estimate

	/*
	 * FILTER.
	 */
	for (t=1; t < NUM_SIMULATIONS; t++) {

		printf("| y(%d,:)",t); vec_print(observations[t],NUM_SENSORS);
		for (i=0; i < NUM_PARTICLES; i++) {
			printf("| x(%d,:,%d)",t,i+1); vec_print(x[i],2);
		}

		/*
		 * MEASURE THE ERROR (for simulation tests only) / for the previous time step 
		 */
		err[t-1] = vec_dist(x_est,x_true[t-1]);

		/*
		 * IMPORTANCE SAMPLING + WEIGHT UPDATE / each step
		 */

		dpf_importance_sampling();

		dpf_weight_update(observations[t]);
			
		/*
		 * ESTIMATE + RESAMPLING
		 */
		dpf_normalize();		// normalise weights
		dpf_estimate(x_est);	// estimate

		printf("| x_est(%d,:) ", t); vec_print(x_est,2);
		printf("| x_pos(%d,:) ", t); vec_print(x_true[t],2);

		dpf_resample(1.0/NUM_PARTICLES);
	}

	/*
	 * FINALISE.
	 */

	t = NUM_SIMULATIONS-1;

	printf("| y(%d,:)",t); vec_print(observations[t],NUM_SENSORS);
	for (i=0; i < NUM_PARTICLES; i++) {
		printf("| x(%d,:,%d)",t,i+1); vec_print(x[i],2);
	}
	err[t] = vec_dist(x_est,x_true[t]);

	printf("| plot(x_pos(:,1),x_pos(:,2),'-ko','MarkerSize',4,'LineWidth',2)\n");
	printf("| hold on\n");
	printf("| xlim([%f,%f])\n", MIN_X, LIM_X);
	printf("| ylim([%f,%f])\n", MIN_Y, LIM_Y);
	printf("| plot(l(:,1),l(:,2),'k-','LineWidth',14)\n");
	printf("| [J d] = size(s);\n");
	printf("| for j=1:J\n");
	printf("|     plot(s(j,1),s(j,2),'ks','MarkerSize',8,'LineWidth',2)\n");
	printf("| end\n");
	printf("| plot(x_est(:,1),x_est(:,2),'ro-','MarkerSize',4,'LineWidth',2)\n");
	printf("| hold off\n");

	double MAE  = vec_sum(err,NUM_SIMULATIONS)/NUM_SIMULATIONS;
	double SDAE = sqrt(vec_var(err,NUM_SIMULATIONS)); 

	printf("[id=%d] ERR  = ", MY_ID); vec_print(err,NUM_SIMULATIONS);
	printf("[id=%d] MAE  = %f\n", MY_ID, MAE);
	printf("[id=%d] SDAE = %f\n", MY_ID, SDAE);

	//write_string(MY_ID,55,MAE);
	//write_string(MY_ID,56,SDAE);
}

int main(int argc, char *argv[]) {

	int seed = ((argc >= 2) ? atoi(argv[1]) : 0);

	printf("Running CPF with seed %d\n", seed);
	sseed(seed);

	gen_true_trajectory();		// generate a trajectory
	gen_observations();			// generate observations
	//load_observations();		// load observations 		(for real data, see layout_bin.h)

	run_cpf(0);
	return 0;
}
