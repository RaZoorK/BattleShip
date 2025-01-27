// Mathis Slawy Lucas Provendier

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define SIZE 12
#define NUM_BOAT 6

typedef struct {
    int size;
    char nom[15];
    char symbol;  // Caractère utilisé pour représenter le bateau
    int hits;
} Bateau;

typedef struct {
    int x;
    int y;
} Coordonnee;

int already_played(Coordonnee coups[], int num_coups, int x, int y) {
    
    for (int i = 0; i < num_coups; i++) {
        if (coups[i].x == x && coups[i].y == y) {
            return 1; // Coordonnées déjà jouées
        }
    }
    return 0; // Coordonnées non jouées
}


void init_grid(char grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = '.';  // Utilise '.' pour représenter les cases vides
        }
    }
}

void show_grid(char grid[SIZE][SIZE]) {
    printf("  |");
    for (int i = 0; i < SIZE; i++) {
        printf("%c ", 'A' + i);
    }
    printf("\n--|-----------------------\n");

    for (int i = 0; i < SIZE; i++) {
        printf("%c |", 'A' + i);
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", grid[i][j]);  // Affiche le contenu de la grille
        }
        printf("\n");
    }
}

// Vérifie si le placement est possible
int possible_placement(char grid[SIZE][SIZE], int size, int x, int y, int horizontal) {
    if (horizontal) {
        if (y + size > SIZE) return 0;  // Vérifie les limites
        for (int i = 0; i < size; i++) {
            if (grid[x][y + i] != '.') return 0;  // Vérifie si la case est libre
        }
    } else {
        if (x + size > SIZE) return 0;  // Vérifie les limites
        for (int i = 0; i < size; i++) {
            if (grid[x + i][y] != '.') return 0;  // Vérifie si la case est libre
        }
    }
    return 1; // Placement possible
}

// Fonction pour placer un bateau sur la grille
void place_boat(char grid[SIZE][SIZE], Bateau *bateau) {
    int x, y, horizontal;
    bateau->hits = 0;  // Initialiser le nombre de segments touchés à 0

    do {
        x = rand() % SIZE;
        y = rand() % SIZE;
        horizontal = rand() % 2;
    } while (!possible_placement(grid, bateau->size, x, y, horizontal));

    if (horizontal) {
        for (int i = 0; i < bateau->size; i++) {
            grid[x][y + i] = bateau->symbol;
        }
    } else {
        for (int i = 0; i < bateau->size; i++) {
            grid[x + i][y] = bateau->symbol;
        }
    }
}

int all_boats_sunk(Bateau boats[NUM_BOAT]) {
    for (int i = 0; i < NUM_BOAT; i++) {
        if (boats[i].hits < boats[i].size) {
            return 0;  
        }
    }
    return 1;  
}

// Vérifie si la case est touchée, coulée ou loupée
char check_collision(char grid[SIZE][SIZE], Bateau boats[NUM_BOAT], char *buf_read) {
    int x = toupper(buf_read[1]) - 'A';  // Extraire les coordonnées depuis buf_read
    int y = toupper(buf_read[0]) - 'A';

    if (grid[x][y] != '.' && grid[x][y] != 'O' && grid[x][y] != 'X') {
        for (int i = 0; i < NUM_BOAT; i++) {
            if (grid[x][y] == boats[i].symbol) {
                boats[i].hits++;  // Incrémenter les segments touchés du bateau
                
                if (boats[i].hits == boats[i].size) {  // Si un bateau est complètement coulé
                    grid[x][y] = 'C';
                    return 'C';
                }
                grid[x][y] = 'X';  // Marquer la case actuelle comme touchée
                return 'X';
            }
        }
    } else if (grid[x][y] == '.') {
        grid[x][y] = 'O';  // Marquer comme raté
        return 'O';  // Retourner 'O' pour indiquer un tir raté
    }
}



void update_grid(char grid[SIZE][SIZE],char *buf_write, char *buf_read)
{
    int x = toupper(buf_write[1]) - 'A';
    int y = toupper(buf_write[0]) - 'A';
    if (strncmp(buf_read, "Victory", 7) == 0)
    {
        grid[x][y] = 'V';
    }
    else if (strncmp(buf_read, "Touché", 6) == 0)
    {
        grid[x][y] = 'X';
    } 
    else if (strncmp(buf_read, "Touché Coulé", 12) == 0)
    {
        grid[x][y] = 'C';
    }
    else if (strncmp(buf_read, "Loupé", 5) == 0)
    {
        grid[x][y] = 'O';
    }
}