# Projet Programmation Réseaux


---

### Composition du groupe 31:

Pierre Amorin 71800495
Patrick Chang 71802046
Vincent Du    71801856

---

### Compilation et Exécution du programme :

- Dans `/pr6_project`, entrez dans le terminal `make` pour compiler le programme

- Ensuite sur plusieurs terminaux, toujours dans le même répertoire,
**Lancez successivement :**
    - `java Manager 4242`
    - `java Diffuser1 config_diff1.txt`
    - `./user1 4242 config_user1.txt`

On peut remplacer `config_diff1.txt` et `config_user1.txt` par les autres fichiers txt de config respectives.
Le port `4242` a été choisi arbitrairement, et a été fixé dans les fichiers config_diff.

### Configurations :

**Pour le diffuseur :**
- Modifiez le fichier en paramètre `config_diff.txt` où l'adresse IPv4 selon
l'adresse où le `Manager` est exécuté.
**Dans l'ordre les lignes représentent :**
    - `port de multicast`
    - `port de reception TCP`
    - `Addresse IPv4 du Manager`
    - `Port de connexion`

**Pour l'user :**
- Modifiez le fichier en paramètre `config_user.txt`
**Dans l'ordre les lignes représentent :**
    - `L'ID du Client` (8 caractères max)
    - `Addresse IP4 du Diffuser`


### Choix :

- Le client `user1.c` est automatisé, il va se connecter à un `Diffuser` disponible au hasard,
puis il va envoyer le message `[LIST]` au `Manager`,
et ensuite il va envoyer au hasard toutes les 8 secondes au `Diffuser` :
    - Soit le message `[MESS]` avec un des messages prédéfini
    - Soit le message `[LAST 003]` pour recevoir les 3 derniers messages reçus

### Répartition du travail :

- Pierre s'est occupé de faire le gestionnaire `Manager1` en `Java`

- Vincent a fait le `Diffuser` en `Java`, ainsi que l'`user` en `C`

- Patrick a fait les `formatter` et `parser` en `C` et en `Java` et a relié avec Vincent 
les fichiers précédents avec ces nouveaux fichiers

- Pierre et Patrick ont ensuite modifié le client et le diffuseur afin de rajouter
les fichiers de configuration nécessaires.
