/* Renvoie 1 si les deux chaînes sont identiques et 0 sinon */
int is_equal (char *s1, char *s2);

int char2int(char c);

int read_pos(char *pos, int* l, int* c);

/* Prend en paramètre une chaîne de la forme "10x20" et les addresses des variables qui stockent les dimensions */
int read_dim(char *dim, int *l, int *h);

int read_pos_ship(char *pos, int *l_debut, int *l_fin, int *c_debut, int *c_fin);

/* Vérifie que les coordonnées de bateau sont valides (renvoie la taille du bateau et 0 si erreur)*/
int check_pos_ship(int largeur, int hauteur, int l_debut, int l_fin, int c_debut, int c_fin, int joueur);

/* Renvoie 1 si la chaîne s2 commence par s1 */
int est_prefixe(char* s1, char* s2);

int len(char *s);

char *copie(char *s);

int joueur_gagnant(int nb_j1, int nb_j2); /* Renvoie le numéro du joueur gagnant et 0 sinon */