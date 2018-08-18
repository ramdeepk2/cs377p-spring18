#include <stdio.h>
#include <stdlib.h>
#include "emmintrin.h"
#include <papi.h>
#include <time.h>

#define NUMCOUNTERS 4
#define CLEARSIZE 5000

void main() {
	int EventSet = PAPI_NULL; // Our event set
	int retval;
	int b = 2;
	int a = 3;
	time_t t;
	srand((unsigned) time(&t));

	// cache clearing
	int *clearer = (int *) malloc(CLEARSIZE * CLEARSIZE * sizeof(int));


	int matrixsizes[8] = {50, 100, 200, 400, 800, 1200, 1600, 2000};
	long long values[NUMCOUNTERS];

	// Initialize PAPI library
	retval = PAPI_library_init(PAPI_VER_CURRENT);
	if (retval != PAPI_VER_CURRENT) {
		fprintf(stderr, "PAPI library initialization error!\n");
		exit(1);
	}

	// Create our EventSet, then add to it
	if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
		fprintf(stderr, "PAPI library initialization error!\n");
		exit(1);
	}


	if (PAPI_add_event(EventSet, PAPI_TOT_INS) != PAPI_OK) {
	  	fprintf(stderr, "PAPI library initialization error!\n");
		exit(1);
	}
	if (PAPI_add_event(EventSet, PAPI_TOT_CYC) != PAPI_OK) {
	  	fprintf(stderr, "PAPI library initialization error!\n");
		exit(1);
	}
	if (PAPI_add_event(EventSet, PAPI_L1_DCA) != PAPI_OK) {
	  	fprintf(stderr, "PAPI library initialization error!\n");
		exit(1);
	}
	if (PAPI_add_event(EventSet, PAPI_L2_DCA) != PAPI_OK) {
	  	fprintf(stderr, "PAPI library initialization error!\n");
		exit(1);
	}


	// if (PAPI_add_event(EventSet, PAPI_L1_DCM) != PAPI_OK) {
	//   	fprintf(stderr, "PAPI library initialization error!   1\n");
	// 	exit(1);
	// }
	// if (PAPI_add_event(EventSet, PAPI_FP_INS) != PAPI_OK) {
	//   	fprintf(stderr, "PAPI library initialization error!   2\n");
	// 	exit(1);
	// }

	// if (PAPI_add_event(EventSet, PAPI_L2_DCM) != PAPI_OK) {
	//   	fprintf(stderr, "PAPI library initialization error!   3\n");
	// 	exit(1);
	// }
	// if (PAPI_add_event(EventSet, PAPI_LST_INS) != PAPI_OK) {
	//   	fprintf(stderr, "PAPI library initialization error!   4\n");
	// 	exit(1);
	// }



	// Generate random matrix
	for(int h = 0; h < 8; h++) {
		int currsize = matrixsizes[h];
		double *matrixMultiply = (double *) malloc(currsize * currsize * sizeof(double));
		double *matrixSolution = (double *) malloc(currsize * currsize * sizeof(double));

		// Generate random matrix
		for(int j = 0; j < matrixsizes[h]; j++) {
			for(int k = 0; k < matrixsizes[h]; k++) {
				matrixMultiply[k+j*matrixsizes[h]] = rand();
			}
		}

		// clear cache
		for(int j = 0; j < CLEARSIZE; j++) {
			for(int k = 0; k < CLEARSIZE; k++) {
				clearer[k + j * CLEARSIZE] = 56;
			}
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");
		
		// count
		if (PAPI_start(EventSet) != PAPI_OK) {
			fprintf(stderr, "PAPI library initialization error!      7\n");
			exit(1);
		}

		// multiply
		for(int i = 0; i < matrixsizes[h]; i++){ // for each row in the first matrix
			for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
				for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
					matrixSolution[j + i * currsize] += matrixMultiply[i * currsize + k] * matrixMultiply[k * currsize + j];
				}
			}
		}

		// read
		if (PAPI_read(EventSet, values) != PAPI_OK) {
			fprintf(stderr, "PAPI library initialization error!     6\n");
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #1\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 misses =\t%lld\nL1 miss rate =\t%.31f%%\nL2 misses =\t%lld\nL2 miss rate =\t%.31f%%\n\n", values[2], (double)values[2] / (double)values[0], values[3], (double)values[3] / (double)values[1]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("total instructions = %lld\ntotal cycles = %lld\nl1 dca = %lld\nl2 dca = %lld\n\n", values[0], values[1], values[2], values[3]);


		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		for(int j = 0; j < CLEARSIZE; j++) {
			for(int k = 0; k < CLEARSIZE; k++) {
				clearer[k + j * CLEARSIZE] = 11;
			}
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if(PAPI_reset(EventSet) != PAPI_OK) {
		    fprintf(stderr, "PAPI library initialization error!    7\n");
			exit(1);
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
			for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
				for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		// read
		if (PAPI_read(EventSet, values) != PAPI_OK) {
			fprintf(stderr, "PAPI library initialization error!     6\n");
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #2\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 misses =\t%lld\nL1 miss rate =\t%.31f%%\nL2 misses =\t%lld\nL2 miss rate =\t%.31f%%\n\n", values[2], (double)values[2] / (double)values[0], values[3], (double)values[3] / (double)values[1]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("total instructions = %lld\ntotal cycles = %lld\nl1 dca = %lld\nl2 dca = %lld\n\n", values[0], values[1], values[2], values[3]);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		for(int j = 0; j < CLEARSIZE; j++) {
			for(int k = 0; k < CLEARSIZE; k++) {
				clearer[k + j * CLEARSIZE] = 9;
			}
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if(PAPI_reset(EventSet) != PAPI_OK) {
		    fprintf(stderr, "PAPI library initialization error!    7\n");
			exit(1);
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");		
		for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
			for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
				for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		// read
		if (PAPI_read(EventSet, values) != PAPI_OK) {
			fprintf(stderr, "PAPI library initialization error!     6\n");
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #3\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 misses =\t%lld\nL1 miss rate =\t%.31f%%\nL2 misses =\t%lld\nL2 miss rate =\t%.31f%%\n\n", values[2], (double)values[2] / (double)values[0], values[3], (double)values[3] / (double)values[1]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("total instructions = %lld\ntotal cycles = %lld\nl1 dca = %lld\nl2 dca = %lld\n\n", values[0], values[1], values[2], values[3]);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		for(int j = 0; j < CLEARSIZE; j++) {
			for(int k = 0; k < CLEARSIZE; k++) {
				clearer[k + j * CLEARSIZE] = 6;
			}
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if(PAPI_reset(EventSet) != PAPI_OK) {
		    fprintf(stderr, "PAPI library initialization error!    7\n");
			exit(1);
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");		
		for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
			for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
				for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		// read
		if (PAPI_read(EventSet, values) != PAPI_OK) {
			fprintf(stderr, "PAPI library initialization error!     6\n");
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #4\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 misses =\t%lld\nL1 miss rate =\t%.31f%%\nL2 misses =\t%lld\nL2 miss rate =\t%.31f%%\n\n", values[2], (double)values[2] / (double)values[0], values[3], (double)values[3] / (double)values[1]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("total instructions = %lld\ntotal cycles = %lld\nl1 dca = %lld\nl2 dca = %lld\n\n", values[0], values[1], values[2], values[3]);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		for(int j = 0; j < CLEARSIZE; j++) {
			for(int k = 0; k < CLEARSIZE; k++) {
				clearer[k + j * CLEARSIZE] = 2;
			}
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if(PAPI_reset(EventSet) != PAPI_OK) {
		    fprintf(stderr, "PAPI library initialization error!    7\n");
			exit(1);
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");		
		for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
			for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
				for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		// read
		if (PAPI_read(EventSet, values) != PAPI_OK) {
			fprintf(stderr, "PAPI library initialization error!     6\n");
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #5\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 misses =\t%lld\nL1 miss rate =\t%.31f%%\nL2 misses =\t%lld\nL2 miss rate =\t%.31f%%\n\n", values[2], (double)values[2] / (double)values[0], values[3], (double)values[3] / (double)values[1]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("total instructions = %lld\ntotal cycles = %lld\nl1 dca = %lld\nl2 dca = %lld\n\n", values[0], values[1], values[2], values[3]);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		for(int j = 0; j < CLEARSIZE; j++) {
			for(int k = 0; k < CLEARSIZE; k++) {
				clearer[k + j * CLEARSIZE] = 3;
			}
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if(PAPI_reset(EventSet) != PAPI_OK) {
		    fprintf(stderr, "PAPI library initialization error!    7\n");
			exit(1);
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");		
		for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
			for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
				for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		// read
		if (PAPI_read(EventSet, values) != PAPI_OK) {
			fprintf(stderr, "PAPI library initialization error!     6\n");
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #6\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 misses =\t%lld\nL1 miss rate =\t%.31f%%\nL2 misses =\t%lld\nL2 miss rate =\t%.31f%%\n\n", values[2], (double)values[2] / (double)values[0], values[3], (double)values[3] / (double)values[1]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("total instructions = %lld\ntotal cycles = %lld\nl1 dca = %lld\nl2 dca = %lld\n\n", values[0], values[1], values[2], values[3]);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if(PAPI_reset(EventSet) != PAPI_OK) {
		    fprintf(stderr, "PAPI library initialization error!    7\n");
			exit(1);
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		// stop counters
		if (PAPI_stop(EventSet, values) != PAPI_OK) {
		    fprintf(stderr, "PAPI library initialization error!    5\n");
			exit(1);
		}
	} 
}