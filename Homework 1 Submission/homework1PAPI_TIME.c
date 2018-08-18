#include <stdio.h>
#include <stdlib.h>
#include "emmintrin.h"
#include <papi.h>
#include <time.h>

#define NUMCOUNTERS 4

void main() {
	// int EventSet = PAPI_NULL; // Our event set
	int retval;
	int b = 2;
	int a = 3;
	time_t t;
	float real_time, proc_time, mflops;
	long long flpins;
	srand((unsigned) time(&t));
	int matrixsizes[8] = {50, 100, 200, 400, 800, 1200, 1600, 2000};
	long long values[NUMCOUNTERS];

	// Initialize PAPI library
	// retval = PAPI_library_init(PAPI_VER_CURRENT);
	// if (retval != PAPI_VER_CURRENT) {
	// 	fprintf(stderr, "PAPI library initialization error!\n");
	// 	exit(1);
	// }



	// Generate random matrix
	for(int h = 0; h < 8; h++) {
		int currsize = matrixsizes[h];
		double *matrixMultiply = (double *)malloc(currsize*currsize*sizeof(double));
		double *matrixSolution = (double *)malloc(currsize*currsize*sizeof(double));

		//double matrixmultiply[matrixsizes[h]][matrixsizes[h]];
		//double matrixSolution[matrixsizes[h]][matrixsizes[h]];
		//generate random matrix
		for(int j = 0; j < matrixsizes[h]; j++) {
			for(int k = 0; k < matrixsizes[h]; k++) {
				matrixMultiply[k+j*matrixsizes[h]] = rand()%15;
				//matrixMultiply[j][k] = rand();
			}
		}

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");
		
		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

		// multiply
		for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
			for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
				for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #1\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("proc Time = %f\nreal time = %f\n\n", proc_time, real_time);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

		for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
			for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
				for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #1\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("proc Time = %f\nreal time = %f\n\n", proc_time, real_time);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}
		
		for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
			for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
				for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #1\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("proc Time = %f\nreal time = %f\n\n", proc_time, real_time);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

	
		for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
			for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
				for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #1\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("proc Time = %f\nreal time = %f\n\n", proc_time, real_time);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}
	
		for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
			for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
				for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #1\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("proc Time = %f\nreal time = %f\n\n", proc_time, real_time);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}
	
		for(int k = 0; k < matrixsizes[h]; k++){ // for each element in the corresponding row of the second matrix
			for(int i = 0; i < matrixsizes[h]; i++){ //for each row in the first matrix
				for(int j = 0; j < matrixsizes[h]; j++){ // for each elements in the first row
					matrixSolution[j+i*currsize] += matrixMultiply[i*currsize+k] * matrixMultiply[k*currsize+j];
					//matrixSolution[i][j] += matrixmultiply[i][k] * matrixmultiply[k][j];
				}
			}
		}

		if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
			exit(1);
		}

		printf("Matrix size = %dx%d. Iteration #1\n", matrixsizes[h], matrixsizes[h]);
		// printf("L1 Data cache misses = %lld    L2 data cache misses = %lld \nFloating Point instructions = %lld    Load Store Instructions = %lld \n\n", 
		// 	values[0], values[2], values[1], values[3]);
		printf("proc Time = %f\nreal time = %f\n\n", proc_time, real_time);

		__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");
	} 
}