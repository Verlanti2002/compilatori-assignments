#include <stdio.h>

const int N = 10;

void loop_fusion1(int A[], int B[]) {
    for(int i=0; i<N; i++)
        A[i] = i + 1;

    for(int i=0; i<N; i++)
        B[i] = A[i];
}

void loop_fusion2(int A[], int B[]) {
    for (int i=0; i<N; i++)
        A[i] = i + 1;
        
    for (int i=0; i<N; i++)
        B[i] = A[i+3] + i;
}

void loop_fusion3(int A[], int B[], int C[], int D[]) {

	for (int i=0; i<N; i++)
	    A[i] = B[i] * C[i];

	for (int i=0; i<N; i++)
		D[i] = A[i] + C[i];

	for (int i=0; i<N; i++)
		A[i] = C[i] - 1;
}