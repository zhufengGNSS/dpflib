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
#include "sleeper.h"		// for msleep

// for global est
double x_local_ests[NUM_NODES][DIM_STATE];
double C[NUM_NODES];
double x_global_est[DIM_STATE];

// =================================================>

void run_dpf(int MY_ID) {

	int SU_ID = (MY_ID + 1) % NUM_NODES;
	printf("[DPF] START mote j=%d, trade mote k=%d. N=%d, J=%d. rand (test) = %f.\n", MY_ID, SU_ID, NUM_NODES, NUM_SENSORS, randn());

	// VARS
	int t = 0, j = 0, n = 0, i = 0;
	double err[NUM_SIMULATIONS];

	printf("\n======= INIT =======\n\n");

	/*
	 * INIT.
	 */
	NUM_PARTICLES = 25;
	dpf_init();								

	/*
	 * INITIAL BROADCAST
	 */
	C[MY_ID] = dpf_normalize(); 											// c: normalization constant (needed for: Estimate, Resampling)
	write_particle(MY_ID,1,x[i], DIM_STATE, w[i], 33);						// broadcast initial best praticle (0th)
	dpf_estimate(x_local_ests[MY_ID]);										// initial estimate
	write_particle(MY_ID,1,x_local_ests[MY_ID], DIM_STATE, C[MY_ID], 44);	// broadcast initial estimate	
	printf(" C[%d] = %f, X[%d] = ", MY_ID, C[MY_ID], MY_ID); vec_print(x_local_ests[MY_ID],2);

	//err[0] = vec_dist(x_local_ests[MY_ID],x_true[0]);		// report error
	msleep(200);

	printf("\n======= FILTER =====\n\n");

	for (t=1; t < NUM_SIMULATIONS; t++) {

		/*
		 * IMPORT DATA
		 */
		// LOCAL ESTIMATES
		printf("\n======= TIMESTEP %d =====\n\n",t);
		for (n=0; n < NUM_NODES; n++) {
			if (n != MY_ID) {
				C[n] = read_particle(n, 1, x_local_ests[n], DIM_STATE, 44);		// in from the network
				//printf("[OK] read c = %f, x_est =", C[n]); vec_print(x_local_ests[n],2);
			}
			else {
				printf("*");
			}
			printf(" C[%d] = %f, X[%d] = ", n, C[n], n); vec_print(x_local_ests[n],2);
		}

		for (n=0; n < NUM_NODES; n++) {
			printf(" PRINT: C[%d] = %f, X[%d] = ", n, C[n], n); vec_print(x_local_ests[n],2);
		}

		/*
		 * GLOBAL ESTIMATE
		 */
		printf("GLOBAL ESTIMATE\n");
		dpf_global_estimate(x_local_ests, C, x_global_est);						// @todo only share/calce x,y
		printf("| x_est(%d,:) ", t); vec_print(x_global_est,2);
		printf("| x_pos(%d,:) ", t); vec_print(x_true[t-1],2);

		// BEST PARTICLE
		w[i] = read_particle(SU_ID,1,x[i],DIM_STATE,33);						// best particle into 'i'

		/*
		 * MEASURE THE ERROR (for simulation tests only)
		 ( for the previous time step )
		 */
		err[t-1] = vec_dist(x_global_est,x_true[t-1]);

		/*
		 * IMPORTANCE SAMPLING + WEIGHT UPDATE 
		 */
		printf("IMPORTANCE SAMPLING\n");
		dpf_importance_sampling();

		printf("WEIGHT UPDATE\n");
		dpf_weight_update(observations[t]);

		/*
		 * ESTIMATE + RESAMPLING 
		 */
		printf("NORMALIZATION\n");
		C[MY_ID] = dpf_normalize(); 	// c: normalization constant (needed for: Estimate, Resampling)

		printf("ESTIMATE\n");
		dpf_estimate(x_local_ests[MY_ID]);

		printf("RESAMPLING\n");
		dpf_resample(C[MY_ID]/NUM_PARTICLES);	// then set weights to c/NUM_PARTICLES afterwards

		/*
		 * BROADCAST
		 */
		printf("BROADCAST");

		msleep(500);

		i = max_index(wnorm,NUM_PARTICLES);										// find i = best particle
		write_particle(MY_ID,1,x[i],DIM_STATE,w[i],33);							// broadcast best particle
		write_particle(MY_ID,1,x_local_ests[MY_ID], DIM_STATE, C[MY_ID], 44);	// broadcast estimate	@todo only x,y

		msleep(500);

		printf("[BROADCAST] c = %f, x_est =", C[MY_ID]); vec_print(x_local_ests[MY_ID],2);


	}

	// we continue to get the final estimate ...
	t = NUM_SIMULATIONS-1;

	/*
	 * IMPORT DATA (*final*)
	 */
	printf("\n======= TIMESTEP %d =====\n\n",t);
	for (n=0; n < NUM_NODES; n++) {
		if (n != MY_ID) {
			C[n] = read_particle(n, 1, x_local_ests[n], DIM_STATE, 44);		// in from the network
		}
		else {
			printf("*");
		}
		printf(" C[%d] = %f, X[%d] = ", n, C[n], n); vec_print(x_local_ests[n],2);
	}

	for (n=0; n < NUM_NODES; n++) {
		printf(" PRINT: C[%d] = %f, X[%d] = ", n, C[n], n); vec_print(x_local_ests[n],2);
	}

	/*
	 * GLOBAL ESTIMATE (*final*)
	 */
	printf("GLOBAL ESTIMATE\n");
	dpf_global_estimate(x_local_ests, C, x_global_est);						// @todo only share/calce x,y
	printf("| x_est(%d,:) ", t); vec_print(x_global_est,2);
	printf("| x_pos(%d,:) ", t); vec_print(x_true[t],2);

	err[0] = 0.03; // @TEMP!!!!!!!!
	err[t] = vec_dist(x_global_est,x_true[t]);

	double MAE  = vec_sum(err,NUM_SIMULATIONS)/NUM_SIMULATIONS;
	double SDAE = sqrt(vec_var(err,NUM_SIMULATIONS)); 

	printf("[id=%d] ERR  = ", MY_ID); vec_print(err,NUM_SIMULATIONS);
	printf("[id=%d] MAE  = %f\n", MY_ID, MAE);
	printf("[id=%d] SDAE = %f\n", MY_ID, SDAE);

	write_val(MY_ID,55,MAE);
	write_val(MY_ID,56,SDAE);

	if (MY_ID == 0) { // all global estimates are the same, write out this one
		FILE *fp;
		fp=fopen("dat/temp.dat", "a");
		fprintf(fp, "[%d] [", MY_ID);
		for (t = 0; t < NUM_SIMULATIONS; t++) {
			fprintf(fp," %.2f", err[t]);
		}
		fprintf(fp," ] : ");
		fprintf(fp,"%f\n", MAE);
		fclose(fp);
	}

	printf("closing now, in 3 seconds, check temp.dat ...\n");
	sleep(3);

}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		printf("Use: %s <id> <seed>\n", argv[0]);
		printf("You must start NUM_NODES instances simulateously, each with the same seed.\n");
		printf("Results are written to temp.dat\n");
		exit(1);
	}

	int id = atoi(argv[1]);
	int seed = ((argc >= 2) ? atoi(argv[2]) : 0);

	sseed(seed);

	gen_true_trajectory();		// generate a trajectory
	gen_observations();			// generate observations
	//load_observations();		// load observations 		(for real data, see layout_bin.h)

	sseed(id+1 * seed*1); // reset seed (we only wanted the path to be identical for each node)

	run_dpf(id);

	return 0;
}
