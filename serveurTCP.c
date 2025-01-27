// Mathis Slawy Lucas Provendier
// serveurTCP.c (serveur TCP)
// gcc -o serveur_TCP.exe serveurTCP.c
// ./serveur_TCP.exe BATTLESHIP 8000

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "grille.c"
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

#define SIZE 12
#define NUM_BOAT 5
#define NBECHANGE 40

char* id = 0;
short port = 0;
int sock = 0; /* socket de communication */
int nb_reponse = 0;
int client_count = 0; // Compteur pour identifier chaque joueur
int active_players = 0; // Compteur pour les joueurs actifs

void handle_sigchld(int sig) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        active_players--;  // Décrémenter le nombre de joueurs actifs
        printf("Un joueur a terminé. Joueurs actifs restants: %d\n", active_players);
        if (active_players == 0) {
            printf("Tous les joueurs ont terminé. Fermeture du serveur.\n");
            close(sock);
            exit(0);  // Fermer le serveur si tous les joueurs ont terminé
        }
    }
}

int main(int argc, char** argv) {
    struct sockaddr_in serveur, client; /* SAP du serveur et du client */
    socklen_t len = sizeof(client);

    // Clear the terminal at the start
    system("clear");

    if (argc != 3) {
        fprintf(stderr, "usage: %s id port\n", argv[0]);
        exit(1);
    }
    id = argv[1];
    port = atoi(argv[2]);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "%s: socket %s\n", argv[0], strerror(errno));
        exit(1);
    }

    serveur.sin_family = AF_INET;
    serveur.sin_port = htons(port);
    serveur.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&serveur, sizeof(serveur)) < 0) {
        fprintf(stderr, "%s: bind %s\n", argv[0], strerror(errno));
        exit(1);
    }

    if (listen(sock, 5) != 0) {
        fprintf(stderr, "%s: listen %s\n", argv[0], strerror(errno));
        exit(1);
    }

    printf("Serveur en attente de connexions sur le port %d...\n", port);

    // Installer le gestionnaire pour SIGCHLD (pour gérer la terminaison des processus enfants)
    signal(SIGCHLD, handle_sigchld);

    while (1) {
        int sock_pipe = accept(sock, (struct sockaddr*)&client, &len);
        if (sock_pipe < 0) {
            fprintf(stderr, "Erreur acceptation: %s\n", strerror(errno));
            continue;
        }

        // Incrémenter le compteur de clients pour attribuer un nom unique
        client_count++;
        active_players++;  // Incrémenter le nombre de joueurs actifs
        char player_id[50];
        sprintf(player_id, "Joueur %d", client_count);

        // Création d'un processus enfant pour gérer chaque client
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Erreur fork: %s\n", strerror(errno));
            close(sock_pipe);
        } else if (pid == 0) {
            // Processus enfant
            close(sock); // L'enfant n'a pas besoin du socket d'écoute

            char grid[SIZE][SIZE]; // Déclaration de la grille
            Bateau boats[NUM_BOAT] = {
                {5, "Porte Avion", 'A', 0},
                {5, "Porte Avion", 'B', 0},
                {3, "Fregate", 'C', 0},
                {3, "Fregate", 'D', 0},
                {3, "Fregate", 'E', 0}
            };

            srand(time(NULL)); 
            init_grid(grid);

            for (int i = 0; i < NUM_BOAT; i++) {
                place_boat(grid, &boats[i]);
            }

            printf("%s (%s) connecté\n", player_id, inet_ntoa(client.sin_addr));

            for (int nb_question = 0; nb_question < NBECHANGE; nb_question++) 
            {
                char buf_read[1<<8], buf_write[1<<8];
                int ret = read(sock_pipe, buf_read, sizeof(buf_read));

                if (ret <= 0) {
                    printf("Erreur lecture de %s (%s): %d: %s\n", player_id, inet_ntoa(client.sin_addr), ret, strerror(errno));
                    break;
                }

                // Affiche le message reçu avec l'identifiant du joueur
                printf("Serveur reçoit: %s de la part de %s (%s)\n", buf_read, player_id, inet_ntoa(client.sin_addr));

                char result = check_collision(grid, boats, buf_read);
                show_grid(grid);
                if (result == 'X') {
                    strcpy(buf_write, "Touché\n");
                } else if (result == 'O') {
                    strcpy(buf_write, "Loupé\n");
                } else if (result == 'C') {
                    strcpy(buf_write, "Touché Coulé\n");
                }

                if (all_boats_sunk(boats) == 1) {
                    strcpy(buf_write, "Victory\n");
                    printf("Le joueur %s (%s) a gagné!\n", player_id, inet_ntoa(client.sin_addr));
                    write(sock_pipe, buf_write, strlen(buf_write) + 1); 
                    break;
                }

                ret = write(sock_pipe, buf_write, strlen(buf_write) + 1);
                if (ret <= strlen(buf_write)) {
                    printf("Erreur écriture de %s (%s): %d: %s\n", player_id, inet_ntoa(client.sin_addr), ret, strerror(errno));
                    break;
                }
            }
            close(sock_pipe);
            exit(0);
        } else {
            // Processus parent : fermer le pipe du socket client et continuer à attendre d'autres connexions
            close(sock_pipe);
        }
    }

    close(sock);
    return 0;
}
