#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void gather(double in, double *out, int root) 
{
    int rang, P, t;
    MPI_Status sta;
    MPI_Status *tab_sta;
    MPI_Request *tab_req;

    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    if (rang == root) 
    {
        tab_sta = (MPI_Status*)malloc(P*sizeof(MPI_Status));
        tab_req = (MPI_Request*)malloc(P*sizeof(MPI_Request));

        tab_req[root] = MPI_REQUEST_NULL;//explication dans le sujet

        out[root] = in;
        for( t = 0 ; t < P ; t++ )
	{
            if (t != root)
                MPI_Irecv(&(out[t]), 1, MPI_DOUBLE, t, 1000, MPI_COMM_WORLD, tab_req+t);
	}

        MPI_Waitall(P, tab_req, tab_sta);

        free(tab_sta);
        free(tab_req);

    } 
    else 
    {
        MPI_Send(&in, 1, MPI_DOUBLE, root, 1000, MPI_COMM_WORLD);
    }
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
    gather(in, out, root);

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

