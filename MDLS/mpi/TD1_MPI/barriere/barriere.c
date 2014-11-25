#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

void barriere() {

    int rang, P, t, msg, tag1, tag2;
    MPI_Status sta;

    MPI_Comm_rank(MPI_COMM_WORLD, &rang);

    tag1 = 1000;
    tag2 = 2000;

    if (rang == 0) {

	MPI_Comm_size(MPI_COMM_WORLD, &P);

	for( t = 1 ; t < P ; t++ )
	    MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, tag1, MPI_COMM_WORLD, &sta);

	for( t = 1 ; t < P ; t++ )
	    MPI_Send(&msg, 1, MPI_INT, t, tag2, MPI_COMM_WORLD);


    } else {

	msg = 0;
	MPI_Send(&msg, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD);
	MPI_Recv(&msg, 1, MPI_INT, 0, tag2, MPI_COMM_WORLD, &sta);
    }
}

int main(int argc, char **argv) {
    
    int rang;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rang);

    usleep(rang*1000000);
    printf("P%d : je vais appeler la barriere\n", rang);

    barriere();

    printf("P%d : je suis sorti de la barriere\n", rang);

    MPI_Finalize();
    return 0;
} 
