#ifndef TYPES
#define TYPES


#define NB_SHIP 16 /* Taille initiale de ship_list */
#define TAILLE_MOT 16 /* Taille initiale d'un mot */
#define NB_MOT 4 /* Nombre de mots par ligne */

#define VIDE '~'
#define MINE '#'
#define TOUCHE '*'


typedef enum {INIT, PLACEMENT, JEU} moment;

typedef enum {NORMAL, INTERACTIF, AUTO} mode;


/* Structure représentant une case du plateau de jeu */
typedef struct {
    char val; /* Le caractère qui sera affiché */
    int l; /* Le numéro de ligne */
    int c; /* Le numéro de colonne */
    struct ship *ship_part; /* Pointeur vers la structure représentant le bateau présent sur cette case */
} tile;


/* Structure représentant un bateau */
typedef struct ship {
    int taille;
    int hp; /* Vaut taille au départ, diminue de 1 si touché et vaut 0 si coulé */
    tile **cases; /* Le tableau contenant les cases où il est */
    char *nom;
    int joueur; /* Le numéro du joueur possédant ce bateau */
} ship;


/* Structure contenant les variables globales */
typedef struct {
    int largeur;
    int hauteur;
    tile **pdj;

    char **ligne; /* Contient la dernière ligne lue (pour pouvoir libérer sa mémoire en cas d'erreur) */
    
    FILE *input; /* Le fichier d'entrée */
    mode mdj; /* Le mode de jeu (2 joueurs, vs IA, lecture d'un fichier) */
    moment avancement; /* Le niveau d'avancement de la partie (INIT : Définition de la taille du plateau)*/
    int tour; /* Le numéro du joueur dont c'est le tour (1 ou 2) */
    int derniere_taille; /* La taille du dernier bateau placé par le joueur 1 */

    ship **ship_list; /* Tableau contenant tous les bateaux */
    int nb_ship; /* Le nombre actuel de bateaux */
    int ship_j1; /* Nombre de bateaux non coulés de j1 */
    int ship_j2;
} state;


#endif