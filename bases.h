#include "types.h"

void sink_ship(ship *pship);

void my_exit(state *pstate, int code);

void mk_pdj(state *pstate, int largeur, int hauteur);

void afficher(state *pstate, int joueur);

ship *find_ship(state *pstate, char *nom);

ship *mk_ship(state *pstate, char *nom, char *pos);

void move_ship(state *pstate, ship *pship, int x, int y);

int shoot(state *pstate, int ligne, int colonne);