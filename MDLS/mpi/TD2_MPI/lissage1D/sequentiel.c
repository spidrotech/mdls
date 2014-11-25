#include <stdio.h>
#include <stdlib.h>

struct info_t {
    int nproc; /* A COMPLETER dans init_info */
    int rang;  /* A COMPLETER dans init_info */

    int ntot;
    int nloc;  /* A COMPLETER dans init_info */

    int ideb;
    int ifin;
};

void init_info(int n, struct info_t *info) {

    info->ntot = n;
    info->ideb = 1;   /* A MODIFIER en parallele */
    info->ifin = n+1; /* A MODIFIER en parallele */

    /* A COMPLETER en parallele */
}

void lire_fichier(const char *nom, double *x, struct info_t *info)  {

    FILE *fd;
    int i;

    fd = fopen(nom, "r");

    for( i = 1 ; i <= info->ntot ; i++ )
	fscanf(fd, "%lf\n", &(x[i]));

    fclose(fd);
}

void ecrire_fichier(const char *nom, double *x, struct info_t *info)  {

    FILE *fd;
    int i;
    double pas;

    pas = 1./info->ntot;

    fd = fopen(nom, "w");

    for( i = info->ideb ; i < info->ifin ; i++ )
	fprintf(fd, "%.6e %.6e\n", i*pas-0.5*pas, x[i]);

    fclose(fd);
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

    ecrire_fichier("lissage.txt", x0, &info);

    free(x);
    free(y);

    return 0;
}

