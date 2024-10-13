#include <stdio.h>
#include <stdlib.h>

#include "stdprof.h"
#include "types.h"
#include "aux.h"


/* Libère la mémoire associée au bateau donné */
void sink_ship(ship *pship) {
    free_prof(pship->cases);
    free_prof(pship->nom);
    free_prof(pship);
}


/* Explique le type d'erreur rencontrée et libère la mémoire avant de quitter */
void my_exit(state *pstate, int code) {
    int i;
    switch (code) {
        case 1 : printf("Error : Ligne entrée syntaxiquement incorrecte\n"); break;
        case 2 : printf("Error : Coordonnées invalides\n"); break;
        case 3 : printf("Error : Cette commande ne doit pas être utilisée maintenant\n"); break;
        case 4 : printf("Error : Ce n'est pas à ce joueur de jouer\n"); break;
        case 5 : printf("Error : Bateau inconnu\n"); break;
        case 6 : printf("Error\n"); break;
    }
    for(i=0;i<NB_MOT;i++) {
        free_prof(pstate->ligne[i]);
    }
    free_prof(pstate->ligne);

    for (i=0; i<pstate->nb_ship; i++) {
        sink_ship(pstate->ship_list[i]);
    }
    free_prof(pstate->ship_list);

    for (i=0; i<pstate->hauteur; i++) {
        free_prof(pstate->pdj[i]);
    }
    free_prof(pstate->pdj);

    printf("\n");
    fclose(pstate->input);
    free_prof(pstate);
    exit(code);
}


void mk_pdj(state *pstate, int largeur, int hauteur) { /* On suppose que les dimensions sont valides (strictement positives)*/
    int i, j;
    tile *ligne;
    tile **pdj = malloc_prof(hauteur*sizeof(tile*));
    for (i=0;i<hauteur;i++) {
        ligne = malloc_prof(largeur*sizeof(tile));
        pdj[i] = ligne;
        for (j=0;j<largeur;j++) {
            ligne[j].val = VIDE;
            ligne[j].c = j;
            ligne[j].l = i;
            ligne[j].ship_part = NULL;
        }
    }
    pstate->largeur = largeur;
    pstate->hauteur = hauteur;
    pstate->pdj = pdj;
}

/* Affiche le plateau (si joueur est différent de 0, les bateaux de l'autre joueur sont masqués) */
void afficher(state *pstate, int joueur) { /* Le joueur pour lequel on affiche le plateau (1 ou 2)*/
    int largeur = pstate->largeur;
    int hauteur = pstate->hauteur;
    tile **pdj = pstate->pdj;

    int i, j;
    char val;

    if (largeur > 10) { /* Affichage des dizaines des numéros de colonne */
        printf("\n%25c", ' ');
        for (i=10;i<largeur;i++) {
            printf("%d ", i/10);
        }
    }
    printf("\n     ");
    for (i=0;i<largeur;i++) {
        printf("%d ", i%10); /* Affichage des numéros de colonne */
    }
    printf("\n   +");
    for (i=0;i<=largeur;i++) { /* Affichage de la bordure haute */
        printf("-+");
    }
    printf("\n");
    for (i=0;i<hauteur;i++) {
        if (i == hauteur/2) printf("   |%*c\n", 2*(largeur+1), '|');
        printf("%*d | ", 2, i);
        for (j=0;j<largeur;j++) {
            val = pdj[i][j].val;
            if ((val == 'A' && joueur == 2) || (val == 'B' && joueur == 1)) printf("~ ");
            else printf("%c ",val); /* Affichage du contenu de la case */
        }
        printf("|\n");
    }
    printf("   +");
    for (i=0;i<=largeur;i++) { /* Affichage de la bordure basse */
        printf("-+");
    }
    printf("\n\n");
}


ship *find_ship(state *pstate, char *nom) {
    int i;
    ship* pship;
    for (i=0;i<pstate->nb_ship;i++) {
        pship = pstate->ship_list[i];
        if (is_equal(pship->nom, nom)) return pship;
    }
    my_exit(pstate,5); /* Il n'y a aucun bateau avec ce nom */
    return NULL;
}


/* Créer un bateau avec le nom donné et le place sur le plateau */
ship *mk_ship(state *pstate, char *nom, char *pos) {
    ship *pship;
    tile **cases;
    int l_debut, c_debut, l_fin, c_fin;
    int taille;
    int i, l, c;
    
    if (! read_pos_ship(pos, &l_debut, &l_fin, &c_debut, &c_fin)) my_exit(pstate,2); /* Si il y a une erreur dans la lecture de la position du bateau */

    taille = check_pos_ship(pstate->largeur, pstate->hauteur, l_debut, l_fin, c_debut, c_fin, pstate->tour);
    if (!taille) my_exit(pstate,2);

    if ((pstate->tour == 2) && (taille != pstate->derniere_taille)) my_exit(pstate,2); /* Si les bateaux de J1 et de J2 ne sont pas de la même taille */

    cases = malloc_prof(taille*sizeof(tile*));
    pship = malloc_prof(sizeof(ship));
    pship->nom = copie(nom);
    pship->taille = taille;
    pship->hp = taille;
    pship->cases = cases;
    pship->joueur = pstate->tour;
    
    l = l_debut; c = c_fin;
    for (i=0;i<taille;i++) {
        if (l_debut == l_fin) c = c_debut + i; /* Bateau horizontal */
        else l = l_debut + i; /* Bateau vertical */
        
        cases[i] = &pstate->pdj[l][c];
        if (cases[i]->ship_part) {
            printf("Un autre bateau est sur le chemin !\n");
            sink_ship(pship);
            my_exit(pstate,2);
        }
        cases[i]->val = pship->joueur - 1 + 'A'; /* A pour J1, B pour J2 */
        pstate->pdj[l][c].ship_part = pship;
    }
    pstate->ship_list[pstate->nb_ship] = pship; /* Le nouveau bateau est ajouté à la liste */
    pstate->nb_ship ++;
    if (pstate->tour == 1) pstate->ship_j1 ++;
    else pstate->ship_j2 ++;
    pstate->derniere_taille = taille;
    printf("Bateau %s de %d:%d à %d:%d' et de longueur %d créé\n", nom, c_debut, l_debut, c_fin, l_fin, taille);
    return pship;
}


/* Renvoie 1 si le bateau peut se déplacer, sinon renvoie 0 et abîme les bateaux en cas de collision */
int can_move(state *pstate, ship *pship, int x, int y) {
    int l, c, i;
    int move = 1;
    if (pship->hp != pship->taille) { /* Si le bateau est touché il ne peut pas se déplacer */
        printf("Bateau %s endommagé, déplacement impossible\n", pship->nom);
        return 0;
    }
    for (i=0;i<pship->taille;i++) {
        l = pship->cases[i]->l; /* Son ordonnée */
        c = pship->cases[i]->c; /* Son abscisse */
        if ((pstate->tour == 1) && (l+y >= pstate->hauteur/2)) {
            printf("Le bateau %s de J1 ne peut pas se déplacer dans la moitié basse !\n", pship->nom);
            return 0; /* Si un bateau de J1 se déplace dans la moitié basse du plateau */
        }
        if ((pstate->tour == 2) && (l+y < pstate->hauteur/2)) {
            printf("Le bateau %s de J2 ne peut pas se déplacer dans la moitié haute !\n", pship->nom);
            return 0; /* Si un bateau de J2 se déplace dans la moitié haute du plateau */
        }
        
        if ((l+y < 0) || (l+y >= pstate->hauteur) || (c+x < 0) || (c+x >= pstate->largeur)) {
            printf("Le bateau %s ne peut pas quitter le plateau !\n", pship->nom);
            return 0; /* Si le bateau se déplace en dehors du plateau il ne se passe rien */

        } else if ((pstate->pdj[l+y][c+x].ship_part != pship) && (pstate->pdj[l+y][c+x].ship_part)) { /* Si on se déplace sur une case d'un autre bateau */
            /* Les deux bateaux entrés en collision sont touchés */
            printf("Collision ! Les bateaux %s et %s sont endommagés\n", pship->nom, pstate->pdj[l+y][c+x].ship_part->nom);
            pstate->pdj[l+y][c+x].val = TOUCHE;
            pstate->pdj[l+y][c+x].ship_part->hp --;
            if (pstate->pdj[l+y][c+x].ship_part->hp == 0) {
                printf("Bateau %s coulé\n", pstate->pdj[l+y][c+x].ship_part->nom);
                if (pstate->pdj[l+y][c+x].ship_part->joueur == 1) pstate->ship_j1 --;
                else if (pstate->pdj[l+y][c+x].ship_part->joueur == 2) pstate->ship_j2 --;
            }

            pstate->pdj[l][c].val = TOUCHE;
            pstate->pdj[l][c].ship_part->hp --;
            if (pstate->pdj[l][c].ship_part->hp == 0) {
                printf("Bateau %s coulé\n", pship->nom);
                if (pstate->pdj[l][c].ship_part->joueur == 1) pstate->ship_j1 --;
                else if (pstate->pdj[l][c].ship_part->joueur == 2) pstate->ship_j2 --;
            }
            move = 0;
        }
    }
    return move;
}


/* Déplace le bateau de x cases horizontalement et y cases verticalement */
void move_ship(state *pstate, ship *pship, int x, int y) {
    int l, c, i, j;
    
    if (can_move(pstate, pship, x, y)) { /* Si il n'y a pas de problèmes pour le déplacement (collision ou sortie du tableau) */
        for (i=0;i<pship->taille;i++) {
            if (x+y <= 0) { /* L'ordre de parcours change en fonction de la direction du déplacement */
                l = pship->cases[i]->l; /* Son abscisse */
                c = pship->cases[i]->c; /* Son ordonnée */
                pship->cases[i] = &pstate->pdj[l+y][c+x];
            } else {
                j = (pship->taille-1)-i;
                l = pship->cases[j]->l;
                c = pship->cases[j]->c;
                pship->cases[j] = &pstate->pdj[l+y][c+x];
            }
            pstate->pdj[l][c].val = VIDE;
            pstate->pdj[l][c].ship_part = NULL;

            if (pstate->pdj[l+y][c+x].val == '#') {
                pstate->pdj[l+y][c+x].val = TOUCHE; /* Si le bateau se déplace sur une mine*/
                pship->hp --;
                if (pship->hp == 0) { /* Si le bateau est coulé */
                    printf("Déplacement sur une mine : bateau %s coulé !\n", pship->nom);
                    if (pship->joueur == 1) pstate->ship_j1 --;
                    else pstate->ship_j2 --;

                } else {
                    printf("Déplacement sur une mine : bateau %s endommagé\n", pship->nom);
                }

            } else pstate->pdj[l+y][c+x].val = pship->joueur-1 + 'A';; /* Vaut A si c'est le tour du joueur 1 et B sinon */
            pstate->pdj[l+y][c+x].ship_part = pship;
        }
    }
}


/* Renvoie 0 si raté, 1 si touché et 2 si coulé */
int shoot(state *pstate, int ligne, int colonne) {
    tile **pdj = pstate->pdj;
    if ((ligne < 0) || (ligne >= pstate->hauteur) || (colonne < 0) || (colonne >= pstate->largeur)) my_exit(pstate,2); /* Coordonnées invalides */

    printf("Tir en %d:%d : ", colonne, ligne);
    if (pdj[ligne][colonne].ship_part == NULL) { /* Si il n'y a pas de bateau sur la case */
        pdj[ligne][colonne].val = MINE;
        printf("Raté !\n");
        return 0;
        
    } else if (pdj[ligne][colonne].val == TOUCHE) {
        printf("Partie de bateau déjà touchée !\n");
        return 0;

    } else {
        pdj[ligne][colonne].val = TOUCHE;
        pdj[ligne][colonne].ship_part->hp --;
        
        if (pdj[ligne][colonne].ship_part->hp == 0) {
            if (pdj[ligne][colonne].ship_part->joueur == 1) pstate->ship_j1 --; /* Si le bateau coulé appartient à J1 */
            else pstate->ship_j2 --;
            printf("Bateau %s touché, coulé !\n", pdj[ligne][colonne].ship_part->nom);
            return 2;

        } else {
            printf("Bateau %s touché !\n", pdj[ligne][colonne].ship_part->nom);
            return 1;
        }
    }
}
