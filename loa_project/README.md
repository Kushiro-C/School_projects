# Projet LOA-M1-2021-2022

### Sujet n°3 (Le jeu)

Membres du groupe :

Patrick Chang 71802046
Vincent Du    71801856

---

## Installation / Compilation

Pour compiler le projet, dans un terminal tapez :

`make`

Puis pour lancer une partie, faire

`./main`

## Quelques tests...

Il y a quelques tests écrits à la main dans un fichier `tester.cpp`

Pour l'exécuter, il faut entrer dans un terminal

`make test`

## Les commandes du jeu

Après avoir lancé le jeu avec `./main`

Vous serez présenté avec des informations à chaque moment du jeu,
 et des choix où il faudra entrer le nombre qui
 est entre parenthèses, correspondant à votre choix.

Par exemple s'il est marqué :

`(2) Faire une jolie action`

Vous entrerez dans votre terminal, tout simplement : `2`

Pour les déplacements (Choix d'Action '4'), nous avons décidé
 d'attribuer aux directions Ouest, Sud, Est, Nord respectivement
 les valeurs 1, 2, 3, 5 (Pas de 4), ce qui est plus intuitif lors
 des déplacements avec un pavé numérique.
 Le chiffre 6 représente l'action "Ne pas se déplacer et termine le tour".

Petite Astuce, pour voir son inventaire on peut voir les objets
 qu'on peut poser (Choix d'Action '3')

---

## Les autres documents (rapport et UML) sont dans le répertoire `docs`

---