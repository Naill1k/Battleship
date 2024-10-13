#include <stdlib.h>
#include <stdio.h>
#include <time.h> /* Pour srand */

#include "stdprof.h"
#include "types.h"
#include "aux.h"
#include "bases.h"


int x0=-1, y0=-1; /* Les coordonnées d'un bateau trouvé */
int x, y; /* Les coordonnées du dernier tir */
int dx=0, dy=0; /* La direction du bateau */
int on_ship = 0; /* Vaut 1 si l'IA est en train de détruire un bateau */

int seed_set = 0; /* Pour n'appeler qu'une seule fois srand */

void place_bateau(state *pstate) {
    int k, n;
    int i, j;
    int di=0, dj=0;
    int x, y;
    ship *pship;
    tile **cases;
    
    if (!seed_set) {
        srand(time(NULL)); /* Pour n'appeler qu'une seule fois srand */
        seed_set = 1;
    }

    i = rand() % pstate->largeur; /* Choisit une case au hasard */
    j = (rand() % (pstate->hauteur / 2)) + pstate->hauteur / 2; /* Seulement dans la moitié basse */
    n = rand() % 2;
    switch (n) { /* On choisit une direction au hasard */
        case 0 : di = 1; break;
        case 1 : dj = 1; break;
    }
    for (k=0; k<pstate->derniere_taille; k++) { /* On regarde si il y a la place de mettre le bateau */
        x = i + k*di;
        y = j + k*dj;
        if ((x < 0) || (x >= pstate->largeur) || (y < pstate->hauteur/2) || (y >= pstate->hauteur) || (pstate->pdj[y][x].val != VIDE)) {
            place_bateau(pstate); /* Si il n'y a pas la place, on essaye à un autre endroit */
            return;
        }
    }
    /* Si il y a la place, on met le bateau */
    cases = malloc_prof(pstate->derniere_taille*sizeof(tile*));
    pship = malloc_prof(sizeof(ship));
    pship->nom = copie("\"B\"");
    pship->taille = pstate->derniere_taille;
    pship->hp = pship->taille;
    pship->cases = cases;
    pship->joueur = pstate->tour;
    
    for (k=0; k<pstate->derniere_taille; k++) {   
        x = i + k*di;
        y = j + k*dj;         
        cases[k] = &pstate->pdj[y][x];
        cases[k]->val = 'B';
        cases[k]->ship_part = pship;
    }
    pstate->ship_list[pstate->nb_ship] = pship; /* Le nouveau bateau est ajouté à la liste */
    pstate->nb_ship ++;
    pstate->ship_j2 ++;
    printf("Bateau de l'IA placé\n");
}


void tir(state *pstate) {
    int res, n, valide=0;

    while (!valide) { /* Coordonnée invalide : case déjà choisie ou sortie de plateau */
        if (on_ship) {
            if ((dx == 0) && (dy == 0)) { /* Si on ne connait pas la direction du bateau */
                n = rand() % 4;
                switch (n) { /* On choisit une direction au hasard */
                    case 0 : dx = 1; break;
                    case 1 : dx = -1; break;
                    case 2 : dy = 1; break;
                    case 3 : dy = -1; break;
                }
            }
            x += dx; y += dy;
            
        } else {
            x = rand() % pstate->largeur;
            y = rand() % (pstate->hauteur / 2); /* On ne tire que dans la moitié haute du plateau */
        }
        /* Si c'est en dehors du plateau ou que l'on a déjà tiré ici */
        if ((x < 0) || (x >= pstate->largeur) || (y < 0) || (y >= pstate->hauteur/2) || (pstate->pdj[y][x].val == TOUCHE) || (pstate->pdj[y][x].val == MINE)) {
            dx = 0; dy = 0;
            x = x0; y = y0;
        } else valide = 1;
    }
    res = shoot(pstate, y, x); /* 0:raté, 1:touché, 2:coulé */

    if ((res == 0) && (on_ship)) { /* Si raté (mauvaise direction) */
        dx *= -1; dy *= -1; /* On part dans l'autre sens */
        x = x0; y = y0; /* Et on se replace à l'endroit où est le bateau */

    } else if ((res == 1) && (!on_ship)) { /* Si on découvre le bateau */
        x0 = x; y0 = y; /* On stocke ses coodonnées */
        on_ship = 1;
        
    } else if (res == 2) { /* Si coulé */
        on_ship = 0;
        dx = 0; dy = 0;
    }
}
