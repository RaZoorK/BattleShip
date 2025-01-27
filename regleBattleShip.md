# Bienvenue dans la bataille navale !
**Règles du jeu :**
1. Vous jouez contre un serveur qui place 5 bateaux sur une grille de taille 12*12
2. Vous devez entrer les coordonnées pour tenter de toucher les bateaux adverses (par exemple, AG, BL, etc.).
3. Le serveur vous répondra avec 'Touché', 'Loupé', ou 'Touché Coulé'.
4. Votre objectif est de couler tous les bateaux.
5. Vous avez 40 tentatives pour réussir à couler tous les bateaux.
**Bonne chance et que la bataille commence !**

**Commande :**
- Pour créer les .exe : 
Client : `gcc -o client_TCP.exe clientTCP.c`<br>
Serveur : `gcc -o serveur_TCP.exe serveurTCP.c`

- Puis pour lancer les clients et les serveurs :
Client : `./clientTCP.exe [nom_du_client] localhost [port]`<br>
Serveur : `./serveurTCP.exe [nom_du_serveur] [port]`

Pour lancer plusieurs clients il suffit de changer le nom du client dans la commande de lancement