#define VAR_VELOCITY 0.004
#define VAR_POSITION 0.1
#define MAX_VELOCITY 0.8
#define STD_DEV_VELOCITY sqrt(VAR_VELOCITY) 
#define STD_DEV_POSITION sqrt(VAR_POSITION) 
#define ANGLE_PER_CHANGE 0.05

/*
 * SET PRIOR.
 * Set a particle 'xp' to its 'prior' + some noise
 *
 * param  	prior[]			// the prior
 * param* 	xp[]			// a particle, which is updated
 */
void set_prior(double xp[], double prior[]) {
	vec_set_randn(xp,4);
	vec_mult(xp,2,STD_DEV_POSITION);
	vec_mult(xp+2,2,STD_DEV_VELOCITY);
	vec_add_vec(xp,4,prior);
}

/*
 * DYNAMIC FUNCTION.
 * Move a particle 'xp' p(x|x_{t-1}) (importance sampling).
 *
 * param* 	xp[]			// a particle, which is updated
 */
void importance_sampling(double xp[]) {

	// MOVE
	xp[0] = xp[0] + xp[2] + randn()*STD_DEV_POSITION;	// ADD PLENTY OF NOISE
	xp[1] = xp[1] + xp[3] + randn()*STD_DEV_POSITION;	// ADD PLENTY OF NOISE
	xp[2] = xp[2] + randn()*STD_DEV_VELOCITY;
	xp[3] = xp[3] + randn()*STD_DEV_VELOCITY;

	// REBOUND 
	// @Note: comment this out when installng to the mote for real life trajectories, to save double calculations 
	// (people can't walk through walls, anyway)
	if (xp[0] > LIM_X) {
		xp[0] = LIM_X + (LIM_X-xp[0]); 	// rebound x
		xp[2] = -xp[2]; 	 			// momentum
	}
	else if (xp[0] < MIN_X) {
		xp[0] = MIN_X + (MIN_X-xp[0]); 	// rebound x
		xp[2] = -xp[2]; 	 			// momentum
	}
	if (xp[1] > LIM_Y) {
		xp[1] = LIM_Y + (LIM_Y-xp[1]); 	// rebound y
		xp[3] = -xp[3]; 				// momentum
	}
	else if (xp[1] < MIN_Y) {
		xp[1] = MIN_Y + (MIN_Y-xp[1]); 	// rebound y
		xp[3] = -xp[3]; 				// momentum
	}

	// CHANGE ANGLE every so often, a particle heads off at a new angle
	if (randu() < ANGLE_PER_CHANGE) { 
		vec_set_angle(xp+2,randu()*6.2832); 	// change angle to theta
		vec_mult(xp+2,2,randu()*MAX_VELOCITY); 	// set a new velocity
	}

}

