#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

int main(int argc, char **argv) {

    int rang, P;
    char str_host[255];

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    gethostname(str_host, 255);
    printf("%d, %d, %s\n", rang, P, str_host);

    MPI_Finalize();

    return 0;
}

