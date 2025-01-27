// Lucas Provendier Mathis Slawy
// clientTCP.c (client TCP)
// gcc -o client_TCP.exe clientTCP.c
// ./client_TCP.exe CLIENT 127.0.0.1 8000


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include "grille.c"
 
#define NBECHANGE 40
 
char* id=0;
short sport=0;
 
int sock=0; /* socket de communication */
 
int main(int argc, char** argv)
{
    struct  sockaddr_in moi; /* SAP du client */
    struct  sockaddr_in serveur; /* SAP du serveur */
    int nb_question=0;
    int ret,len;
    char grid[SIZE][SIZE]; // Déclaration de la grille
    Coordonnee coups[NBECHANGE];  
    int num_coups = 0;  // Nombre de coups joués

    system("clear");

    if (argc!=4) {
        fprintf(stderr,"usage: %s id serveur port\n",argv[0]);
        exit(1);
    }
    id= argv[1];
    sport= atoi(argv[3]);
 
    if ((sock = socket (AF_INET, SOCK_STREAM ,0)) == -1) {
        fprintf(stderr,"%s: socket %s\n",argv[0],
            strerror(errno));
        exit(1);
    }
    serveur.sin_family = AF_INET;
    serveur.sin_port = htons(sport);
    inet_aton(argv[2],&serveur.sin_addr);
    
    if (connect(sock, (struct sockaddr*)&serveur,sizeof(serveur))<0) {
        fprintf(stderr,"%s: connect %s\n",argv[0],strerror(errno));
        perror("bind");
        exit(1);
    }
    len=sizeof(moi);
    getsockname(sock,(struct  sockaddr  *)&moi,&len);
 
    printf("Bienvenue dans la bataille navale !\n");
    printf("Règles du jeu :\n");
    printf("1. Vous jouez contre un serveur qui place 5 bateaux sur une grille de taille %d x %d.\n", SIZE, SIZE);
    printf("2. Vous devez entrer les coordonnées pour tenter de toucher les bateaux adverses (par exemple, A5, B2, etc.).\n");
    printf("3. Le serveur vous répondra avec 'Touché', 'Loupé', ou 'Touché Coulé'.\n");
    printf("4. Votre objectif est de couler tous les bateaux.\n");
    printf("5. Vous avez %d tentatives pour réussir à couler tous les bateaux.\n", NBECHANGE);
    printf("Bonne chance et que la bataille commence !\n\n");

    init_grid(grid);

    for (nb_question=0 ; nb_question < NBECHANGE ; nb_question++) 
    {
        char buf_read[1<<8], buf_write[1<<8];
        int x, y;

        printf("\n");
        printf("Renseignez les coordonnées souhaitées : \n");
        do {
            scanf("%s", buf_write);
            x = toupper(buf_write[1]) - 'A';  // Extraire les coordonnées
            y = toupper(buf_write[0]) - 'A';

            // Vérifier que l'entrée contient au moins deux caractères
            if (strlen(buf_write) < 2) {
                printf("Format incorrect. Veuillez entrer deux caractères (ex: AG, BL).\n");
            }

            // Vérifier les limites des coordonnées (ex: A-L pour les lettres, 0-11 pour les chiffres)
            else if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
                printf("Coordonnées hors limites. Veuillez entrer des coordonnées valides (ex: AL, BG).\n");
                
            }

            if (already_played(coups, num_coups, x, y)) {
                printf("Coordonnées déjà jouées. Veuillez entrer d'autres coordonnées.\n");
            }
        } while (already_played(coups, num_coups, x, y) || (x < 0 || x >= SIZE || y < 0 || y >= SIZE) ||(strlen(buf_write) < 2));  // Redemander si déjà joué

        // Ajouter les coordonnées jouées dans le tableau
        coups[num_coups].x = x;
        coups[num_coups].y = y;
        num_coups++;

        ret=write(sock,buf_write,sizeof(buf_write));
		if (ret<=strlen(buf_write)) {
			printf("\n%s: erreur dans write (num=%d, mess=%s)\n",
				argv[0],ret,strerror(errno));
			continue;
		}

        ret=read(sock, buf_read, sizeof(buf_read));
        if (ret<=0) {
			printf("\n%s:  erreur dans read (num=%d, mess=%s)\n",
				argv[0],ret,strerror(errno));
			continue;
		}
        
        update_grid(grid, buf_write, buf_read);

        show_grid(grid);
       
        printf("%s", buf_read);
        if (strncmp(buf_read, "Victory", 7) == 0) {
            printf("Félicitations, vous avez gagné !\n");
            close(sock);  // Ferme la connexion si le joueur a gagné
            break;  // Sort de la boucle de jeu
        }

    }
    if(!close(sock))
    {
        printf("Défaite, tu as perdu\n");     
    }
    close(sock);
    return 0;
}