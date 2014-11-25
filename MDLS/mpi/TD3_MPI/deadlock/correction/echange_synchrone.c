#include <stdio.h>                                                            
#include <stdlib.h>                                                             
#include <mpi.h>

void echange_synchrone(int rang, char *buf, char *buf2, int n) {
    MPI_Status sta;
    int vois = (rang+1) % 2;

    if (rang == 0) {
	MPI_Ssend(buf,  n, MPI_BYTE, vois, 0, MPI_COMM_WORLD);
	MPI_Recv(buf2, n, MPI_BYTE, vois, 0, MPI_COMM_WORLD, &sta);
    } else {
	MPI_Recv(buf2, n, MPI_BYTE, vois, 0, MPI_COMM_WORLD, &sta);
	MPI_Ssend(buf,  n, MPI_BYTE, vois, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char **argv) {

    int n, rang, P;
    char *buf, *buf2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    if (argc != 2) {
	if (rang == 0)
	    printf("Utilisation : %s <n>\n", argv[0]);
	abort();
	return 1;
    }
    n = atoi(argv[1]);

    if (P != 2) {
	if (rang == 0)
	    printf("Il faut exactement 2 processus MPI et non %d\n", P);
	abort();
	return 1;
    }

    buf  = calloc(n, sizeof(n));
    buf2 = calloc(n, sizeof(n));

    printf("Debut echange %d octets\n", n);
    echange_synchrone(rang, buf, buf2, n);
    printf("Fin echange.\n");

    free(buf);
    free(buf2);

    MPI_Finalize();

    return 0;
}
