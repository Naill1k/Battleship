#include "stdprof.h"
#include <stdio.h>


/* Renvoie 1 si les deux chaînes sont identiques et 0 sinon */
int is_equal (char *s1, char *s2) {
    int i=0;
    while (1) {
        if (s1[i] != s2[i]) {
            return 0;

        } else if (s1[i] == '\0') {
            return 1;
        }
        i++;
    }
}


/* Convertit un caractère représentant un chiffre en l'entier correspondant (renvoie -1 en cas d'erreur) */
int char2int(char c) {
    if ((c < '0') || (c > '9')) {
        return -1;

    }else {
        return c-'0';
    }
}


/* Prend en paramètre une chaîne représentant la position et stocke la ligne et la colonne dans les variables données */
int read_pos(char *pos, int *l, int *col) {
    int i=0;
    int *x = col;
    char c;
    *l = 0;
    *col = 0;
    while ((c=pos[i]) != '\0') {
        if (c == ':') {
            if (x != col) {
                return 0;
            } else {
                x = l;
            }

        } else if (char2int(c) == -1) { /* Si le caractère n'est pas un chiffre */
            return 0;

        } else {
            *x = *x * 10 + char2int(c);
        }
        i++;
    }
    return 1;
}


/* Prend en paramètre une chaîne représentant les dimensions et stocke la largeur et la hauteur dans les variables données */
int read_dim(char *dim, int *l, int *h) {
    int i=0;
    int *x = l;
    char c;
    while ((c=dim[i]) != '\0') {
        if (c == 'x') {
            if (x != l) {
                return 0;
            } else {
                x = h;
            }

        } else if (char2int(c) == -1) { /* Si le caractère n'est pas un chiffre */
            return 0;

        } else {
            *x = *x * 10 + char2int(c);
        }
        i++;
    }
    if ((*l <= 0) || (*h <= 0) || (*h%2 != 0)) return 0; /* Il faut un nombre pair de lignes */
    return 1;
}


/* Stocke les coordonnées du bateau dans les variables données (renvoie 0 en cas d'erreur et 1 sinon) */
int read_pos_ship(char *pos, int *l_debut, int *l_fin, int *c_debut, int *c_fin) {
    int i=0;
    int *x = c_debut;
    *l_debut = 0;
    *l_fin = 0;
    *c_debut = 0;
    *c_fin = 0;
    while (pos[i] != '\0') {
        if (pos[i] == '-') {
            if (x == c_debut) x = c_fin;
            else if (x == l_debut) x = l_fin;
            else return 0;
            
        } else if (pos[i] == ':') {
            if (x == c_debut) *c_fin = *c_debut; /* Si le bateau est sur la même colonne */
            else if (x != c_fin) return 0;
            x = l_debut;

        } else if (char2int(pos[i]) == -1) { /* Si le caractère n'est pas un chiffre */
            return 0;

        } else {
            *x = *x * 10 + char2int(pos[i]);
        }
        i++;
    }
    if (x == l_debut) *l_fin = *l_debut; /* Si le bateau est sur la même ligne */
    return 1;
}


/* Vérifie que les coordonnées de bateau sont valides (renvoie la taille du bateau et 0 si erreur)*/
int check_pos_ship(int largeur, int hauteur, int l_debut, int l_fin, int c_debut, int c_fin, int joueur) {
    int taille;
    if (((l_fin-l_debut) != 0) && ((c_fin-c_debut) != 0)) return 0; /* Si les deux sont non nuls */

    taille = 1 + l_fin-l_debut + c_fin-c_debut;

    if (taille <= 0) return 0;

    if ((c_debut < 0) || (c_fin < 0) || (c_debut >= largeur) || (c_fin >= largeur)) return 0;
    if ((l_debut < 0) || (l_fin < 0) || (l_debut >= hauteur) || (l_fin >= hauteur)) return 0;
    
    if ((joueur == 1) && (l_fin >= hauteur/2)) return 0;  /* Si J1 essaie de placer dans la moitié basse */
    if ((joueur == 2) && (l_debut < hauteur/2)) return 0; /* Si J2 essaie de placer dans la moitié haute */

    return taille;
}


/* Renvoie 1 si la chaîne s2 commence par s1 */
int est_prefixe(char *s1, char *s2) {
    int i=0;
    while (s1[i] != '\0') {
        if (s1[i] != s2[i]) return 0;
        i++;
    }
    return 1;
}


int len(char *s) {
    int l = 0;
    while (s[l] != '\0') l++;
    return l;
}


char *copie(char *s1) {
    int i;
    int longueur = len(s1);
    char *s2 = malloc_prof(longueur*sizeof(char));
    for (i=0;i<=longueur;i++) s2[i] = s1[i];
    return s2;
}

/* Renvoie le numéro du joueur gagnant et 0 sinon */
int joueur_gagnant(int nb_j1, int nb_j2) {
    if (nb_j1 == 0) {
        printf("Victoire de J2 !\n");
        return 2;
        
    } else if (nb_j2 == 0) {
        printf("Victoire de J1 !\n");
        return 1;
    }
    return 0;
}