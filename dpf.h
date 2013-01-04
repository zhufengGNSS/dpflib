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

#ifndef DPF_H
#define DPF_H

#include <math.h>
#include "observation_model_bin.h" 
#include "dynamic_model.h" 
#include "utils.h"

// Semi-constants (may be changed before initialisation)
#define MAX_NUM_PARTICLES 100 					// NUM OF PARTICLES (PER MOTE)
#define NUM_NODES 4 							// NUM OF PROCESSING ELEMENTS
int NUM_PARTICLES = MAX_NUM_PARTICLES;
double WEIGHT_INIT = 1.0 / MAX_NUM_PARTICLES;

// PF global variables
double w[MAX_NUM_PARTICLES];
double wnorm[MAX_NUM_PARTICLES];
double x[MAX_NUM_PARTICLES][DIM_STATE];
double xtemp[MAX_NUM_PARTICLES][DIM_STATE];
double x_my_est[DIM_STATE];

/*
 * INIT
 * @updates	x[]		all particles are set to an initial state, with some positional noise
 * @updates	w[]		all weights are set to the initial weight 1.0 / NUM_PARTICLES
 */
void dpf_init() {

	int i;
	double w_init = 1.0 / NUM_PARTICLES;
	for (i = 0; i < NUM_PARTICLES; i++) {
		set_prior(x[i],x_init); // set an init. state for particle x[i] (assume that the model knows about the dimensions)
		w[i] = w_init;		  // init particle weight i
	}
}

/**
 * IMPORTANCE SAMPLING
 * @updates	x[]		all particles are importance sampled (moved)
 */
void dpf_importance_sampling() {

	int i;
	for (i = 0; i < NUM_PARTICLES; i++) {
		importance_sampling(x[i]);
	}
}

/**
 * WEIGHT UPDATE
 * We measure how good each particle is according to the given observations
 * @param	y		the observations
 * @updates	w[]		all w[i] += L(x[i] | y)
 */
void dpf_weight_update(double y[]) {

	int i, j;
	double L[NUM_SENSORS]; 		// likelihoods for nodes j = 1 ... NUM_SENSORS

	for (i = 0; i < NUM_PARTICLES; i++) {
		// 2.- Weight update: Weight w_j is set to the likelihood of particle x_j.
		for (j = 0; j < NUM_SENSORS; j++) {
			//                        p (x | y = 1.0)    : p(x | y = 0.0)
			L[j] = (y[j] >= 1.0) ? likelihood(x[i],j) : 1.0 - likelihood(x[i],j);
			//L[j] = (L[j] <= 0.0) ? 0.00001 : L[j]	// @note: check for 0 @may shouldn't be necessary
		}
		w[i] = vec_prod(L,NUM_SENSORS);				
		//w[i] = w[i] * vec_prod(L,NUM_SENSORS);	// @note: we do resampling each time, so no need for w[i]+=
	}
}

/*
 * LOCAL ESTIMATION
 * The weighted average of all local particles; = sum(x * wnorm).
 * @todo could reuse code in the global estimate
 *
 * @param	x[]			particles
 * @param	wnorm[]		normalized weights
 * @param*	x_est[]		the estimate				
 */
void dpf_estimate(double x_est[]) {
	int i, n;
	// set to 0
	vec_set(x_est,DIM_STATE,0.0);					// important: must zero out the array!
	for (i = 0; i < NUM_PARTICLES; i++) {
		for (n = 0; n < DIM_STATE; n++) {
			x_est[n] += (x[i][n] * wnorm[i]);
		}
	}
}


/* 
 * RESAMPLING (Basically 'Bagging' / Weighted sampling with replacement.) 
 *
 * @param	w_reinit	the value to reinitialise all weights with after resampling
 * 						(in a CPF this should just be 1/NUM_PARTICLES i.e. w_init, in a DPF this should be c/NUM_PARTICLES)
 * @updates	x[]			the particles sampled
 * @updates	w[] 		initialised to w_reinit
 * @updates	xtemp[]		the temporary particle array
 */
void dpf_resample(double w_reinit) {
	int i, k;

	// bagging procedure (sample x into xtemp)
	for (i=0; i < NUM_PARTICLES; i++) {

		// used normalised weights (wnorm) as a pdf
		k = randd(wnorm,NUM_PARTICLES);
		// copy particle k into position i
		vec_set_vec(xtemp[i],DIM_STATE,x[k]);
	}

	// copy everything back again (xtemp into x)
	for (i=0; i < NUM_PARTICLES; i++) {

		vec_set_vec(x[i],DIM_STATE,xtemp[i]);
		// ... and reset the weights!
		w[i] = w_reinit;
	}
}

/*
 * 'SYSTEMATIC' (FAST) RESAMPLING
 * A faster version of resampling; take NUM_PARTICLES samples from wnorm.
 */
void dpf_resample_fast(double w_reinit) {
	int i = 0, k = 0;

	// bagging procedure (sample x into xtemp)
	double interval = (double)1.0/(NUM_PARTICLES);
	double f = randu()*interval;
	double cum = wnorm[k++];
	for (f; f < 1.0; f+=interval) {
		while (f >= cum) {
			cum += wnorm[k++];
		}
		vec_set_vec(xtemp[i++],DIM_STATE,x[k]);
	}

	// copy everything back again (xtemp into x)
	for (i=0; i < NUM_PARTICLES; i++) {

		vec_set_vec(x[i],DIM_STATE,xtemp[i]);
		// ... and reset the weights!
		w[i] = w_reinit;
	}
}

/* 
 * GLOBAL ESTIMATION
 * Global estimate given a set of local estimates.
 *
 * @param	x_local_ests[NUM_NODES][DIM_STATE]		// local estimates from each node
 * @param	C[NUM_NODES]							// normalization constants from each node
 * @param* 	x_global_est[DIM_STATE]					// the global estimate
 */
void dpf_global_estimate(double x_local_ests[][DIM_STATE], double C[], double x_global_est[]) {
	int j, n;
	// We create a super-normalisation constant C_ to normalise the normalisation constants
	double C_ = 1.0/vec_sum(C,NUM_NODES);			// super constant
	vec_set(x_global_est,DIM_STATE,0.0); 			// important: zero out x_global_est!
	// calculate the super constant
	for (j=0; j < NUM_NODES; j++) {
		// normalize the normalizing constants
		for (n=0; n < DIM_STATE; n++) {				// multiplied by the _normalised_ super constant
			x_global_est[n] += (x_local_ests[j][n] * (C[j] * C_));
		}
	}
	// @note: if resampling is not done at each step, some code has been dropped here.
}

/*
 * NORMALIZE
 * Normalise the weights.
 *
 * @updates wnorm[] with the normalised weights of w[]
 * @returns	c		the normalization constant used
 */
double dpf_normalize() {
	double c;
	vec_copy(wnorm,w,NUM_PARTICLES); 
	c = vec_sum(wnorm,NUM_PARTICLES);
	vec_div(wnorm,NUM_PARTICLES,c);
	return c;
}

/*
 * GENERATE TRAJECTORY
 * Randomly amble in the area of detection (according to the dynamic model).
 *
 * @updates x_true[] for timesteps t = 0,...,NUM_SIMULATIONS
 */
void gen_true_trajectory() {
	int t=0;
	// the init starting point
	vec_set_vec(x_true[0],DIM_STATE,x_init);
	// the rest
	for (t=1; t<NUM_SIMULATIONS; t++) {
		// the particle moves according to the importance sampling function: p(x_t|x_{t-1})
		vec_set_vec(x_true[t],DIM_STATE,x_true[t-1]);
		importance_sampling(x_true[t]);
	}
}

#endif
