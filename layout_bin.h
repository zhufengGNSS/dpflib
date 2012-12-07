// DIMENSIONS
#define DIM_STATE 4
#define X 0
#define Y 1
#define NUM_SENSORS 10
#define NUM_SIMULATIONS 19

// COORDINATES
#define WIDTH 0.24
#define MIN_X 0.00
#define LIM_X 3.20
#define LIM_Y 6.00
#define MIN_Y 0.0

// ======== OBSERVATION SETTINGS ============
double x_init[DIM_STATE] = {2.5, 0.4, -0.2, 0.2};			// REAL TRAJECTORY + SIMULATIONS
double x_true[NUM_SIMULATIONS][DIM_STATE];
double observations[NUM_SIMULATIONS][NUM_SENSORS];
double lightSource[2][2] = {
	//  x1,  y1,
	{  0.70,  0.0 }, 
	//  x2,  xy
	{  2.50, 0.0},
};
double sensorPositions[NUM_SENSORS][2] = { 
	//  x ,  y1,
	{1.2,	 LIM_Y  },	// 3
	{MIN_X,	 1.8   },	// 4
	{LIM_X,	 5.4   },	// 5
	{0.4,	 LIM_Y  },	// 6
	{LIM_X,	 1.8   },	// 8
	{2.0,	 LIM_Y  },	// 9
	{MIN_X,	 5.4   },	// 10
	{2.8,	 LIM_Y  },	// 11
	{MIN_X,	 3.6   },	// 12
	{LIM_X,	 3.6   },	// 14

};
// ==========================================

double sign(double a[], double b[], double c[]) {
	return ((a[0] - c[0]) * (b[1] - c[1]) - (b[0] - c[0]) * (a[1] - c[1]));
}

int in_triangle(double p[], double a[], double b[], double c[]) {
	// ...
	int b1 = sign(p,a,b) < 0.0;
	int b2 = sign(p,b,c) < 0.0;
	if (b1 != b2) // @temp if b1!=b2, return already
		return 0;
	return ((b1==b2) && (b2==(sign(p,c,a) < 0.0)));
	//int b3 = sign(p,c,a) < 0.0;
	//return ((b1==b2) && (b2==b3));
}

// @temp much faster than in-triangle
int in_square(double pp[], double sp[], double l1[], double l2[]) {
	if (pp[1] > sp[1])				// above the sensor
		return 0;
	if (pp[0] < fmin(sp[0],l1[0]))	// left of both
		return 0;
	if (pp[0] > fmax(sp[0],l2[0]))	// right of both
		return 0;
	if (pp[1] < l1[1])				// below floor
		return 0;
	return 1;
}

int observationsData[NUM_SENSORS][NUM_SIMULATIONS] = {
 {0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
 {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
 {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
};

void load_observations() {
	int t, j;
	for (t=0; t<NUM_SIMULATIONS; t++) {
		//printf("true x[t=%d] ", t); vec_print(x_true[t],DIM_STATE);
		for(j=0; j<NUM_SENSORS; j++) {
			observations[t][j] = observationsData[j][t];
		}
	}
}
