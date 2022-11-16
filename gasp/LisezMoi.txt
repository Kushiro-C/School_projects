# Projet Grammaires et Analyse Syntaxique

*(README.md est pareil que LisezMoi.txt)*

---

### Membres du groupe :

Patrick Chang 71802046
Vincent Du    71801856

---

### Compilation et Exécution du programme :

- Dans `/gasp`, entrez dans le terminal `make` pour compiler le programme

- Ensuite entrez dans le terminal `./run < test/[FICHIER]` où `/test` 
est un répertoire qui contient plusieurs fichiers tests, et `[FICHIER]` 
à remplacer par l'un des fichiers du répertoire.
A noter que `/test/mytest` est un petit test d'affichage personnel.


### Répartition du travail :

- Patrick s'est d'abord occupé de créer les fichiers de bases,
afin de pouvoir lire et afficher le contenu d'un fichier.
On a interprété les déclarations et les instructions de base selon la grammaire,
ainsi que l'évaluation des expressions, et la sauvegarde des variables.

- Vincent a ensuite pris le relais, et a relié la partie interprétation
avec la partie graphique, et a rajouté les instructions conditionnelles
et des boucles, ainsi que les 2 premières extensions.


### Extensions :

On a ajouté les 2 premières extensions qui sont :

- Ajout des instructions `ChangeCouleur color` et `ChangeEpaisseur expression`
à noter que `color` n'est défini que dans les couleurs de base du module Graphics.

- Ajout de l'instruction `Si expression Alors instruction`
sans la condition `Sinon instruction`, tout en restant LR(1)


### Autres choix :

Le sujet veut qu'on place notre curseur aux coordonnées (0,0) avec orientation
vers le haut, mais pour des raisons de visibilité, on a choisi les coordonnées (5,5).
Le choix de la taille de la fenêtre étant arbitraire, elle est à `1200x800`.

Pour afficher notre interface de Graphics, on interprète d'abord,
et crée une nouvelle liste d'instructions purement visuelles,
qu'on envois à notre interface, c'est-à-dire que l'interface va lire `instruction`
pour `Si expression Alors instruction`, avec `expression` qui vaut 0 par exemple.
(Une instruction non attendue est bien évidemment envoyée et attrapée)
