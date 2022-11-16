# Projet TSH - Systèmes L3 2020-2021

#### Groupe 16 (CHANG Patrick DU Vincent ALKHAER Hadi)


### Le But du Projet TSH

Pour ce projet en `C`, on a eu comme but de coder un shell qui 
fonctionne approximativement comme un shell `bash` basique, 
à la différence que pour ce projet, notre shell `tsh` doit traiter 
les fichiers d'archives `.tar` (`tarballs`), comme des répertoires.

Certaines commandes dont `cat`, `cd`, `cp`, `exit`, `ls`, `mkdir`, `mv`, 
`pwd`, `rm`, `rmdir`, doivent donc fonctionner normalement, mais également 
avec les fichiers archives `tarballs`.

---

### Mode d'Emploi

- On a mis à disposition un Dockerfile pour tester notre projet avec 
la même distribution qu'on a utilisé (`Ubuntu 18.04`).

- Pour créer une image Docker, on tape la commande (peut prendre du temps) :
`sudo docker build -t img_projet_sy5 .`

- Ensuite on lance l'image Docker qu'on vient de créer avec :
`sudo docker run -ti img_projet_sy5`


**Maintenant on se trouve dans le Docker, et on peut lancer notre programme**

- On se dirige vers le dossier du projet, `cd home/projet_SY5_Gr16`

- Pour compiler le programme, il suffit de taper la commande `make` 
dans le terminal.

- Pour lancer le programme, après avoir compilé, on entre `./tsh` 
dans le terminal.

- Pour lancer les tests sur string_parser, on entre `make test`, 
puis ./src/tests/string_parser


---


**Autre chose**

Au cours de notre projet, vers la date du `rendu1`, nous avons perdu contact 
avec notre camarade ALKHAER Hadi. Nous lui avons envoyé plusieurs messages, 
malheureusement sans réponses, on s'inquiète un peu à son sujet, au vu de 
cette crise sanitaire, en espérant que le pire ne lui est pas arrivé.
