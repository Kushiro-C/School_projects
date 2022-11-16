Installation d'OCaml sur vos machines
=====================================

A partir du TP6 (exercices sur le morption), et pour le projet de 
ce cours, il vous faudra disposer du compilateur OCaml et de la 
bibliothèque `graphics` en local.

  - Si vous utilisez une machine des salles 2031/2032 (PC/Linux), 
    pas de souci, tout y est. Les Mac de la salle 2003 fournissent
    également OCaml, mais privilégiez plutôt vos propres ordinateurs.

  - Sur vos propres machines, tout dépend du système:
    * Sur une Ubuntu ou Debian : `sudo apt install ocaml ocaml-findlib`
    * Les autres Linux fournissent presque tous ocaml : https://ocaml.org/docs/install.html#Linux
    * Sur Mac et Windows, privilégier une machine virtuelle Linux si vous avez ça. 
      Sinon suivre les instructions de https://ocaml.org/docs/install.html .

  - Depuis chez vous, une autre possibilité est de se connecter
    au serveur de l'UFR nommé `lulu` par ssh, [détails ici](http://www.informatique.univ-paris-diderot.fr/wiki/doku.php/wiki/howto_connect).
    Ce serveur propose les mêmes programmes que les PC/Linux des salles
    2031/2032, et donc en particulier tout ce qu'il faut pour programmer
    en OCaml. En s'y connectant via `ssh -Y` vous pourrez même lancer
    des programmes graphiques, tant qu'ils ne sont pas trop gourmands
    en bande passante, et que vous avez un [serveur X](https://fr.wikipedia.org/wiki/X_Window_System)
    de votre côté.

## Graphics

Sur MacOS, pour une installation native, commencer par installer le
serveur X nommé [XQuartz](https://www.xquartz.org/).

Si votre version d'OCaml est 4.08 ou moins, la bibliothèque `graphics` 
est fournie avec l'installation standard d'OCaml, a priori rien à faire de plus. 
C'est normalement le cas sur les Ubuntu et Debian actuelles, et probablement 
la plupart des autres Linux en ce moment. Attention à bien installer
le paquet `ocaml` et pas seulement `ocaml-nox` vu que ce dernier est
précisément OCaml *sans* graphics (nox c'est pour "No-X11").

Par contre si vous avez OCaml 4.09 ou plus récent, il faudra installer `graphics` 
en plus. Votre système le propose peut-être comme "paquet" déjà installable, 
à vous de voir. Par exemple sous Ubuntu/Debian cela s'appellera un jour 
`libgraphics-ocaml-dev`. Sinon, une manière de faire est d'installer `opam`, voir 
https://ocaml.org/docs/install.html#OPAM  puis lancer
`opam install graphics ocamlfind`.

## Dune

L'outil `dune` est très commode pour superviser la compilation d'un programme OCaml. 
Sur Debian Buster, normalement `sudo apt install dune`. Sur Ubuntu 20.04 ce paquet
s'appelle maintenant `ocaml-dune`, et il n'est malheureusement pas fourni par Ubuntu
18.04. Dans ce cas, et sur les autres systèmes, faire `opam install dune ocamlfind` après avoir
installé et configuré [opam](https://ocaml.org/docs/install.html#OPAM).

Remarque : sur Ubuntu 18.04, même `opam` commence à dater, mieux vaut éviter la version 1
qui n'est plus maintenue. Voir les [conseils suivants](https://opam.ocaml.org/doc/Install.html#Ubuntu)
pour installer `opam` depuis un "PPA" plutôt que la version d'origine du système.

Attention : Ubuntu 18.04 fournit un paquet nommé `whitedune` qui n'a **rien à voir**
avec OCaml et ce projet. Mieux vaut le **désinstaller** si vous l'avez installé par
erreur, pour éviter toute confusion.

## Editeur de code

Mon favori est `emacs` équipé du plugin `tuareg` (paquet nommé `tuareg-mode` 
sous Ubuntu et Debian), et éventuellement du plugin `merlin`. Sinon il y a bien 
d'autres éditeurs de code proposant un support pour OCaml, à vous de voir.

## Plus d'informations

Revoir le cours 3 sur les [Outils](slides/cours-03-outils.md) disponibles dans 
l'écosystème OCaml.
