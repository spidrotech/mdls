#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void gather(double in, double *out, int root,int nbproc) 
{
    MPI_Gather(&in, 1, MPI_double,
out, nbproc, MPI_double, root, MPI_COMM_WORLD); 
}

int main(int argc, char **argv) {

    int rang, P, root, t;
    double in;
    double *out;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    /* Pour changer, on ne prend pas 0 comme racine */
    root = P/2;

    if (rang == root) 
    {
        out = (double*)malloc(P*sizeof(double));
    } 
    else 
    {
        out = NULL;
    }

    in = 1. / (1. + (double)rang);
    gather(in, out, root,P);

    if (rang == root)
    {
	int is_passed = 1;

	for( t = 0 ; t < P ; t++ )
	{
	    if (out[t] != (1. / (1. + (double)t)))
	    {
		printf("out[t%d] != %g\n", t, (1. / (1. + (double)t)) );
		is_passed = 0;
	    }
	}
	printf("%s\n", (is_passed ? "PASSED" : "FAILED"));
    }

    if (out)
    {
	free(out);
    }

    MPI_Finalize();

    return 0;
}

