#include <stdio.h>                                                            
#include <stdlib.h>                                                             
#include <mpi.h>

void echange_bufferise(int rang, char *buf, char *buf2, int n) {
    MPI_Status sta;
    int buff_size;
    char *buff_mpi;
    int vois = (rang+1) % 2;

    buff_size = n + MPI_BSEND_OVERHEAD;
    buff_mpi = malloc(buff_size);

    MPI_Buffer_attach(buff_mpi, buff_size);

    MPI_Bsend(buf,  n, MPI_BYTE, vois, 0, MPI_COMM_WORLD);
    MPI_Recv(buf2, n, MPI_BYTE, vois, 0, MPI_COMM_WORLD, &sta);

    MPI_Buffer_detach(&buff_mpi, &buff_size);
    free(buff_mpi);
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
    echange_bufferise(rang, buf, buf2, n);
    printf("Fin echange.\n");

    free(buf);
    free(buf2);

    MPI_Finalize();

    return 0;
}
