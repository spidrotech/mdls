#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <mpi.h>

struct info_t {
    int nproc; /* A COMPLETER dans init_info */
    int rang;  /* A COMPLETER dans init_info */

    int ntot;
    int nloc;  /* A COMPLETER dans init_info */

    int ideb;
    int ifin;
};

void init_info(int n, struct info_t *info) {

    int Q, R;

    info->ntot = n;

    /* A COMPLETER en parallele */
    MPI_Comm_rank(MPI_COMM_WORLD, &(info->rang));
    MPI_Comm_size(MPI_COMM_WORLD, &(info->nproc));

    Q = n / info->nproc;
    R = n % info->nproc;

    if (info->rang < R) {

	info->nloc = Q+1;
	info->ideb = 1 + info->rang * (Q+1);
	info->ifin = info->ideb + info->nloc;

    } else {

	info->nloc = Q;
	info->ideb = 1 + R * (Q+1) + (info->rang - R) * Q;
	info->ifin = info->ideb + info->nloc;
    }

    printf("nproc = %3d, rang = %3d, ntot = %4d, nloc = %4d, [ideb, ifin[ = [%4d, %4d[\n", 
	    info->nproc, info->rang, info->ntot, info->nloc, info->ideb, info->ifin);
}

void lire_fichier(const char *nom, double *x, struct info_t *info)  {

    FILE *fd;
    int i;

    fd = fopen(nom, "r");

    for( i = 1 ; i <= info->ntot ; i++ )
	fscanf(fd, "%lf\n", &(x[i]));

    fclose(fd);
}

void ecrire_fichier(char *nom, double *x, struct info_t *info)  {

    int tag = 1000;

    if (info->rang == 0)
    {
	MPI_Status sta;
	FILE *fd;
	char nom_proc[1024];
	int i, iproc, ideb_proc, nloc_proc, Q, R;
	double pas;

	pas = 1./info->ntot;

	sprintf(nom_proc, "%s.txt", nom);
	fd = fopen(nom_proc, "w");


	/* Pour avoir les resultats dans l'integralité et dans l'ordre
	identique au sequentiel, on ecrit successivement les resultats
	du processus 0 (deja present en memoire),
	puis du processus 1 (a recevoir),
	puis du processus 2 (a recevoir),
	etc...
	 */

	/* Les resultats du processus 0 sont les resultats du processus local
	   */
	for( i = info->ideb ; i < info->ifin ; i++ )
	    fprintf(fd, "%.6e %.6e\n", i*pas-0.5*pas, x[i]);

	Q = info->ntot / info->nproc;
	R = info->ntot % info->nproc;
	ideb_proc = info->ifin;
	for( iproc = 1 ; iproc < info->nproc ; iproc++ )
	{
	    /* On receptionne les resultats du processus iproc
	       */
	    nloc_proc = (iproc < R ? Q+1 : Q);

	    MPI_Recv(x+ideb_proc, nloc_proc, MPI_DOUBLE, iproc, tag, MPI_COMM_WORLD, &sta);

	    for( i = ideb_proc ; i < ideb_proc+nloc_proc ; i++ )
		fprintf(fd, "%.6e %.6e\n", i*pas-0.5*pas, x[i]);

	    ideb_proc += nloc_proc;
	}

	fclose(fd);

	printf("Ecriture fichier %s\n", nom_proc);
    }
    else
    {
	MPI_Send(x+info->ideb, info->nloc, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
    }
}

void bords(double *x, struct info_t *info) {

    x[0] = (x[1] + x[2]) / 2.;
    x[info->ntot+1] = (x[info->ntot] + x[info->ntot-1]) / 2.;
}

void lissage(double *x0, struct info_t *info, double *x1) {

    int i;

    bords(x0, info);

    for( i = info->ideb ; i < info->ifin ; i++ )
	x1[i] = (x0[i-1] + x0[i] + x0[i+1]) / 3.;
}

#define KMAX 20

int main(int argc, char **argv) {

    int n, k;
    double pas;
    double *x, *y, *x0, *x1, *tmp;
    struct info_t info;

    MPI_Init(&argc, &argv);

    const char *donnees = argv[1]; /* le fichier des donnees bruitees */
    n = atoi(argv[2]); /* nombre de donnees bruitees */

    pas = 1./n;

    init_info(n, &info);

    x = (double*)malloc((n+2)*sizeof(double));
    y = (double*)malloc((n+2)*sizeof(double));

    lire_fichier(donnees, x, &info);

    x0 = x;
    x1 = y;
    for( k = 1 ; k <= KMAX ; k++ ) {
	lissage(x0, &info, x1);
	tmp = x0;
	x0 = x1;
	x1 = tmp;
    }

    char *nom = basename(argv[0]);
    ecrire_fichier(nom, x0, &info);

    free(x);
    free(y);

    MPI_Finalize();

    return 0;
}

