/*
 * NETWORK.
 * These functions aren't actually network functions. We simulate the network by reading and writing files.
 */

#include <stdio.h> 

int read_buf(char fname[], double buffer[], int N) {
	// ================== NETWORK ===================
	FILE *fp;
	size_t count;
	if((fp=fopen(fname, "rb"))==NULL) {
		//printf ("Cannot open file %s\n", fname);
		return -1;
	}
	if ((count = fread(buffer, sizeof(double), N, fp)) != N) {
		if (feof(fp)) {
			printf ("Cannot read everything from file (only %zu) \n", count);
			return 0;
		}
		printf ("Error reading from file\n");
		return -1;
	}
	printf("%s", fclose(fp) == 0 ? "." : "ERROR");
	return 1;
	// ==============================================
}

int write_buf(char fname[], double buffer[], int N) {
	// ================== NETWORK ===================
	FILE *fp;
	size_t count;
	if((fp = fopen(fname, "wb"))==NULL) {
		printf("Cannot open file %s\n", fname);
		return -1;
	}
	if(fwrite(buffer, sizeof(double), N, fp) != N) {
		printf("Cannot write everything to file\n");
		return -1;
	}
	printf("%s", fclose(fp) == 0 ? "." : "ERROR");
	// ==============================================
	return 1;
}

// 0. write value 'val' to file "p"id-key
int write_val(int id, int key, double val) {
	// ================== NETWORK ===================
	FILE *fp;
	char filename[30];
	char buffer[20];
	sprintf(filename,"dat/p%d-%d-%d.dat",id,0,key);
	sprintf(buffer,"%f\n",val);
	fp = fopen(filename, "w");
	fwrite(buffer, 1, strlen(buffer), fp);
	fclose(fp);
	return 0;
}

// 1. write sample (x[],w) to file "p"id-t-key
void write_particle(int id, int t, double x[], int N_x, double w, int key) {
	int i = 0, c = 0;
	int N_s = N_x+1;
	double data[N_s];
	// copy the best particles out
	for (i=0; i < N_x; i++) {
		data[c++] = x[i];
	}
	data[c] = w;
	char filename[30];
	sprintf(filename,"dat/p%d-%d-%d.dat",id,t,key);
	write_buf(filename,data,N_s);
}

// 2. read particle x[] from file "p"id-t-key, and return its w
double read_particle(int id, int t, double x[], int N_x, int key) {
	// declare vars
	int n = 0, c = 0, d = 0;
	int N_s = N_x+1;
	char filename[30];
	sprintf(filename,"dat/p%d-%d-%d.dat",id,t,key);
	double data[N_s];
	// try to open file and dread in particles
	while ((d = read_buf(filename,data,N_s)) <=0) {
		// failed to read %s\n\t(there was nothing, not enough of it, or failed to read ... wait a sec and try again)
		printf("(!))", filename);
		sleep(1);
	}
	// copy the particles into the appropriate gaps
	for (n=0; n < N_x; n++) {
		x[n] = data[c++];
	}
	// and return the weight
	return data[c];
}

