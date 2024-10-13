#include <stdio.h>
#include <stdlib.h>

#include "stdprof.h"
#include "types.h"
#include "bases.h"
#include "aux.h"
#include "ia.h"


/* Analyse la ligne si c'est une action d'un joueur */
void analyse_action(state *pstate, char **ligne) {
    ship *pship;
    int dx=0;
    int dy=0;
    int l, c;
    if ((pstate->tour == 1) ^ (is_equal(ligne[0], "J1"))) my_exit(pstate,4); /* Si ce n'est pas au bon joueur de jouer */
    
    if (is_equal(ligne[1], "P")) {
        if (pstate->avancement != PLACEMENT) my_exit(pstate,3);
        if (ligne[2][0] == '"') mk_ship(pstate, ligne[2], ligne[3]);  /* Si le bateau a un nom */
        else if (pstate->tour == 1) mk_ship(pstate, "\"A\"", ligne[2]);
        else mk_ship(pstate, "\"B\"", ligne[2]);

    } else if (is_equal(ligne[1], "T")) {
            if (pstate->avancement != JEU) my_exit(pstate,3);
            read_pos(ligne[2], &l, &c);
            shoot(pstate, l, c);

    } else {
        if (pstate->avancement != JEU) my_exit(pstate,3);

        else if (is_equal(ligne[1], "H")) dy = -1;
        else if (is_equal(ligne[1], "B")) dy = 1;
        else if (is_equal(ligne[1], "G")) dx = -1;
        else if (is_equal(ligne[1], "D")) dx = 1;
        else my_exit(pstate,1);

        if (ligne[2][0] == '"') { /* Si le bateau est désigné par son nom */
            pship = find_ship(pstate, ligne[2]);
        } else {
            if (! read_pos(ligne[2], &l, &c)) my_exit(pstate,2);
            if ((l < 0) || (c < 0) || (l > pstate->hauteur) || (c > pstate->largeur)) my_exit(pstate,2);
            pship = pstate->pdj[l][c].ship_part;
        }
        if ((pship == NULL) || (pship->joueur != pstate->tour)) my_exit(pstate, 5); /* Si on essaie de déplacer un bateau appartenant à l'autre joueur */
        move_ship(pstate, pship, dx, dy);
    }
    pstate->tour = !(pstate->tour - 1) + 1; /* Le tour devient celui de l'autre joueur */
}


/* Parse la ligne stockée dans le tampon et appelle les fonctions correspondantes */
void analyse_ligne(state *pstate, char **ligne) {
    int l=0;
    int h=0;
    /* On vérifie que l'on ne définit la taille du plateau que sur la première ligne */
    if ((pstate->avancement == INIT) ^ is_equal(ligne[0], "Projet")) my_exit(pstate,3);

    if (is_equal(ligne[0], "Projet")) {
        if (! read_dim(ligne[1], &l, &h)) my_exit(pstate,2); /* Récupération de la largeur et de la hauteur */
        pstate->avancement = PLACEMENT;
        mk_pdj(pstate, l, h);
        afficher(pstate, 0);

    } else if (is_equal(ligne[0], "Afficher")) {
        if (ligne[1] == NULL) afficher(pstate, 0);
        else if (is_equal (ligne[1], "J1")) afficher(pstate, 1);
        else if (is_equal (ligne[1], "J2")) afficher(pstate, 2);

    } else if (is_equal(ligne[0], "Jouer")) {
        if ((pstate->avancement != PLACEMENT) || (ligne[1] != NULL)) my_exit(pstate,3);
        if (pstate->tour != 1) my_exit(pstate,4); /* J2 aurait un bateau de moins que J1 */
        pstate->avancement = JEU;

    } else if (is_equal(ligne[0], "J1") || is_equal(ligne[0], "J2")) {
        analyse_action(pstate, ligne);

    } else my_exit(pstate,1); /* Commande inconnue */
}


/* Lit une ligne de l'entrée spécifiée et l'analyse (renvoie 0 en fin de fichier) */
int split_line(state *pstate) {
    int taille_mot = TAILLE_MOT; /* La taille de base d'un mot */
    char *mot = malloc_prof(taille_mot*sizeof(char));
    char **liste = calloc_prof(NB_MOT, sizeof(mot));
    int i_mot=0; /* Indice du mot actuel */
    int l=0; /* Indice de la lettre dans le mot actuel */
    int in_nom=0; /* Vaut 1 si les caractères lus sont dans un nom de bateau (pour éviter de couper le nom du bateau) */
    char c;
    FILE *save;
    pstate->ligne = liste;
    liste[0] = mot;

    if ((pstate->avancement == JEU) && (joueur_gagnant(pstate->ship_j1, pstate->ship_j2))) { /* Si il y a un gagnant */
        afficher(pstate, 0);
        return 0;
    }
    while (((c=getc(pstate->input)) != '\n') && (c != '#')) {
        if (c == EOF) return 0; /* Si c'est la fin du fichier on renvoie NULL pour sortir de la boucle while de main */
        if (i_mot == NB_MOT) my_exit(pstate,1); /* Si il y a plus de NB_MOT mots sur une ligne, elle est incorrecte */
        if ((c == ' ') && (in_nom == 0)) { /* A la fin d'un mot (qui ne fait pas partie d'un nom de bateau) */
            while ((c=getc(pstate->input)) == ' '); /* On avance jusqu'au mot suivant */

            if (c == '#') {
                while (((c=getc(pstate->input)) != '\n') && (c != EOF)); /* On avance jusqu'à la fin de la ligne */
            }
            if (c == '\n') break;
            if (c == EOF) return 0;
            else {
                mot[l] = '\0';
                liste[i_mot] = mot;
                taille_mot = TAILLE_MOT;
                mot = malloc_prof(taille_mot*sizeof(char));
                i_mot++;
                l = 0;
            }
        } else if (l == taille_mot-2) { /* Si la taille allouée pour le mot est trop petite, elle est doublée */
            taille_mot *= 2;
            mot = realloc_prof(mot, taille_mot);
        }
        if (c == '"') { /* Si on est au début ou à la fin d'un nom de bateau */
            in_nom = !in_nom; /* La valeur passe de 0 à 1 ou de 1 à 0 */
        }
        mot[l] = c;
        l++;
    }
    mot[l] = '\0';
    liste[i_mot] = mot;
    analyse_ligne(pstate, liste);
    if (pstate->mdj == NORMAL) save = fopen("current.txt", "a"); /* On ouvre le fichier */
    for(i_mot=0;i_mot<NB_MOT;i_mot++) {
        if (liste[i_mot] != NULL) {
            if (pstate->mdj == NORMAL) {
                fputs(liste[i_mot], save); /* Et on ajoute la ligne lue */
                fputc(' ', save);
            }
            free_prof(liste[i_mot]);
        }
    }
    if (pstate->mdj == NORMAL) { /* Ajout de la fin de ligne et fermeture du fichier */
        fputc('\n', save);
        fclose(save);
    }
    free_prof(liste);
    return 1;
}


/* Initialise les variables globales */
state *init(int argc, char* argv[]) {
    state *pstate;
    FILE *input;
    mode mdj;
    switch (argc) {
        case 1 :
            input = stdin;
            mdj = NORMAL;
            break;

        case 2 :
            if (is_equal(argv[1], "-i")) {
                input = stdin;
                mdj = INTERACTIF;
            } else exit(1);
            break;

        case 3 :
            if (is_equal(argv[1], "-f")) {
                input = fopen(argv[2], "r");
                if (input == NULL) exit(1); /* Si le fichier n'existe pas */
                mdj = AUTO;
            } else exit(1);
            break;
    }
    if (mdj == NORMAL) system("rm current.txt 2>/dev/null"); /* Suppression de la précédente partie en cours */

    pstate = malloc_prof(sizeof(state)); /* Définition de la structure contenant les variables globales */
    pstate->avancement = INIT;
    pstate->tour = 1;
    pstate->ship_list = malloc_prof(NB_SHIP*sizeof(ship*)); /* Taille initiale de 8 bateaux chacun */
    pstate->nb_ship = 0;
    pstate->ship_j1 = 0;
    pstate->ship_j2 = 0;
    pstate->input = input;
    pstate->mdj = mdj;
    return pstate;
}


int main (int argc, char* argv[]) {
    state *pstate = init(argc, argv);
    printf("Bonjour, veuillez entrer les dimensions du plateau avant de placer vos bateaux.\n");
    printf("J1 ne peut placer ses bateaux que dans la moitié haute et J2 dans la moitié basse.\n");
    printf("Que le meilleur gagne !\n\n");

    while (split_line(pstate)) { /* Boucle d'exécution du programme */
        if ((pstate->mdj == INTERACTIF) && (pstate->tour == 2)) {
            printf("\n");
            if (pstate->avancement == PLACEMENT) place_bateau(pstate);
            else tir(pstate);
            pstate->tour = 1;
        }
        printf("\n");
    }
    
    my_exit(pstate,0);
    return 0;
}
