#include <stdio.h>
#include <stdlib.h>

#include "headers.h"


// load diagonalization routine from lapack
// this method diagonalizes double precision real valued symmetric matrices
// reference: http://physics.oregonstate.edu/~landaur/nacphy/lapack/routines/dsyev.html
extern void dsyev_(char* jobz, char* uplo, int* n, double* a, int* lda, 
				  double* w, double* work, int* lwork, int* info);


/********************************************************************/


#pragma acc routine
void transpose(double *A, int N) {
	int unsigned i, j;
	double element;

	#pragma acc loop parallel
	for (i = 0; i < N; i++) {
		for (j = i; j < N; j++) {
			element = A[i + j * N];
			A[i + j * N] = A[j + i * N];
			A[j + i * N] = element;
		}
	}
}


#pragma acc routine
void rotate(double *A, double *eigvect, int N) {
	double *h;
	h = (double *) malloc(sizeof(double) * N * N);
	matrix_mul_real(A, eigvect, h, N);
	transpose(eigvect, N);
	matrix_mul_real(eigvect, h, A, N);
	transpose(eigvect, N);

	free((void*) h);
}



void diagonalize(double *A, double *D, int N) {
	int n = N, info, lwork;
	double wkopt;
	double* work;
	int LDA = N;

	// we need to get the optimal work load first
	lwork = -1;
	dsyev_("Vectors", "Upper", &n, A, &LDA, D, &wkopt, &lwork, &info);
	// then we perform the actual diagonalization
	lwork = (int)wkopt;
	work = (double*) malloc(lwork * sizeof(double));
	dsyev_("Vectors", "Upper", &n, A, &LDA, D, work, &lwork, &info);
//	free((void*) work);
}