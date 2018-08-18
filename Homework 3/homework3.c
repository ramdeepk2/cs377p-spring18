#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <papi.h>


double **Allocate2DArray_Ofdouble(int x, int y)
{
    int TypeSize = sizeof(double);
    double **ppi = malloc(x*sizeof(double*));
    double *pool = malloc(x*y*TypeSize);
    unsigned char *curPtr = (char *)pool;
    int i;
    if (!ppi || !pool)
    {  /* Quit if either allocation failed */
        if (ppi) free(ppi);
        if (pool) free(pool);
        return NULL;
    }
    /* Create row vector */
    for(i = 0; i < x; i++)
    {
	    *(ppi + i) = (double *)curPtr;
        curPtr += y*TypeSize;
   	}
    return ppi;
}


//Do an ikj matrix multiply of an NxN matrix
void **IKJMatrixMultiply(double **matA, double **matB, double **result, int n){
	int EventSet = PAPI_NULL;
	long long flpins;
	float real_time, proc_time, mflops;
	int retval;
	long long values[1];
	//retval = PAPI_library_init(PAPI_VER_CURRENT);
	__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

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

	if (PAPI_add_event(EventSet, PAPI_FP_OPS) != PAPI_OK) {
	  	fprintf(stderr, "PAPI library initialization error!\n");
		exit(1);
	}
	
	if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
		exit(1);
	}

	__asm__("cpuid" :"=a"(b) :"0"(a) :"%ebx","%ecx","%edx");

	if (PAPI_start(EventSet) != PAPI_OK) {
			fprintf(stderr, "PAPI library initialization error!      7\n");
			exit(1);
	}

	int rowcolSize = n;
	for(int i = 0; i < rowcolSize; i++){
		for(int k = 0; k < rowcolSize; k++){
			for(int j = 0; j < rowcolSize; j++){
				*(*(result+i)+j) += (*(*(matA+i)+k))*(*(*(matB+k)+j)); 
			}
		}
	}

	if (PAPI_read(EventSet, values) != PAPI_OK) {
		fprintf(stderr, "PAPI library initialization error!     6\n");
		exit(1);
	}

	if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK) {
		exit(1);
	}
	printf("For matrix size %d \nproc Time = %f\n FP_OPS = %lld\nmflops = %f\n\n", n, proc_time, values[0], mflops);

}



void main(int argc, char **argv){

	int size = 3;
	double **testMatrix = Allocate2DArray_Ofdouble(size, size);
	double **testMatrix2 = Allocate2DArray_Ofdouble(size, size);
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			*(*(testMatrix+i)+j) = i*j;
			*(*(testMatrix2+i)+j) = i*j;
		}
	}
	double **result = Allocate2DArray_Ofdouble(size, size);

	IKJMatrixMultiply(testMatrix, testMatrix2, result, size);

	printf("\n");
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			printf("%f   ", *(*(result+i)+j));
		}
		printf("\n");
	}
}