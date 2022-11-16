# Introduction à la programmation fonctionnelle

Ce dépôt git contient les ressources pédagogiques du cours de troisième
année de licence d'Informatique de l'Université de Paris (ex-Diderot) intitulé
"Programmation Fonctionnelle" (PF5).

## Objectifs du cours

Voir [OBJECTIFS.md](OBJECTIFS.md)

## Organisation du cours

- Le responsable du cours est Pierre Letouzey. 
- Les chargés de TD sont Balthazar Bauer, Giovanni Bernardi, Adrien Guatto, Pierre Letouzey et Vincent Padovani.
- Le cours est évalué par un projet (30%) et par un examen final (70%).
- **Dates** :
  * Examen présentiel le **mardi 5/1 15h30-17h30**. Programme : tout sauf le cours 12. Documents papiers autorisés.
  * Projet à finir d'ici à **jeudi 14/1 à midi** au plus tard. Rendu via dépôt git sur `gaufre`, voir les consignes dans la section Projet.
  * [Liste provisoire des dépôts](https://www.irif.fr/~letouzey/pf5/depots.txt) qui seront pris en compte comme rendus. Plus de détails dans ce [message](https://listes.u-paris.fr/wws/arc/l3.pf5.info/2021-01/msg00000.html).

## Inscrivez-vous !

- Inscrivez-vous sur https://moodle.u-paris.fr/course/view.php?id=1641
- La mailing-liste dédiée au cours est `l3.pf5.info@listes.u-paris.fr`. Inscrivez-vous via https://listes.u-paris.fr/wws/subscribe/l3.pf5.info


## Cours en ligne (ex-Amphis)

Les amphis initialement prévus pour ce cours **sont suspendus** au vu des contraintes sanitaires actuelles.
Ils sont remplacés par des enseignements en ligne :
  - Des documents pédagogiques à lire/visionner chaque semaine, voir plus bas.
  - Des séances en ligne de questions/réponses sur ces documents : **jeudi 11h-12h**.
  - Le lien BBB pour ces séances est https://bbb-front.math.univ-paris-diderot.fr/recherche/pie-rcp-d5b-tvw
  - Venez poser vos questions au moment que vous voulez (ou pouvez) dans ces créneaux.
  - Vous pouvez aussi me posez vos questions via la mailing-list (cf ci-dessous) ou via mon [email](http://www.irif.fr/~letouzey)

Beaucoup de contenus proviennent ou s'inspirent d'anciens enseignants de ce cours, en particulier de Yann Régis-Gianas (merci!).

**Videos**:
  - Le lien [BBB](https://bbb-front.math.univ-paris-diderot.fr/recherche/pie-rcp-d5b-tvw) ci-dessus propose aussi une liste des captures vidéos brutes des séances questions/réponses passées (incluant quelques cafouillages du prof, vive le direct).
  - Un [autre dépôt](https://www.irif.fr/~letouzey/pf5/videos) pour ces vidéos (au format webm, visible *et* téléchargeable, merci [bbb-downloader](https://github.com/trahay/bbb-downloader), mais sans les messages de chat).

#### Cours 1 (semaine du 9/9) :
  - Un document d'[introduction au fonctionnel et à OCaml](slides/cours-01-yann.pdf) par Yann Régis-Gianas. 
  - Un tour d'horizon d'OCaml : [sketch](https://sketch.sh/s/H3xyXu6P3YdaHMqOVYXq6b/) dynamique et [markdown](slides/cours-01-tour.md) statique.
  - Quelques [questions/réponses](faq/faq1.md).

#### Cours 2 (semaine du 14/9) :
  - Les **types de données** usuels d'OCaml : [sketch](https://sketch.sh/s/RjxDVUFPNMiZqKxDtzdezN/), [markdown](slides/cours-02-types.md).
  - Questions/réponses : [sketch](https://sketch.sh/s/nhihzKwLxmobjB0TDbEeKk/), [fichier OCaml](faq/faq2.ml), vidéo [BBB](https://bbb-front.math.univ-paris-diderot.fr/recherche/pie-rcp-d5b-tvw) et [webm](https://www.irif.fr/~letouzey/pf5/videos)

#### Cours 3 (semaine du 21/9) :
  - **Outils OCaml** (compilateurs, etc) : [markdown](slides/cours-03-outils.md) et démo vidéo [BBB](https://bbb-front.math.univ-paris-diderot.fr/recherche/pie-rcp-d5b-tvw) et [webm](https://www.irif.fr/~letouzey/pf5/videos).

#### Cours 4 (semaine du 28/9)
  - **Fonctions de 1ère classe et programmation d'ordre supérieur** : [sketch4a](https://sketch.sh/s/XjV2RE6tIUAJkvdfQ1rgFN/), [sketch4b](https://sketch.sh/s/tDqsDWq7jwLNCLPX3mzky7/), [markdown](slides/cours-04-fun.md).
  - Questions/réponses du [30/9](https://sketch.sh/s/iHoll1bLeBUb3LCn5Hw22U/) et du [1/10](https://sketch.sh/s/JFcq7Uv7yfl5BtcobMwcWe/) et les [liens](https://bbb-front.math.univ-paris-diderot.fr/recherche/pie-rcp-d5b-tvw) [videos](https://www.irif.fr/~letouzey/pf5/videos) habituels.
  - Une [FAQ](faq/faq3.md) de plus avec quelques faux-amis du débutant OCaml. Attention aux erreurs classiques...

#### Cours 5 (semaine du 5/10)
  - **Récursivité terminale** : [sketch](https://sketch.sh/s/6k9ft6DS3nA6xQjVNa4v1g/), [markdown](slides/cours-05-tailrec.md).
  - Questions/réponses du [7/10](https://sketch.sh/s/PFfSXHi1Swq166PNO4W7lK/) et [8/10](https://sketch.sh/s/xqq4NfSAyoChnDhjTGemTS/) et [liens](https://bbb-front.math.univ-paris-diderot.fr/recherche/pie-rcp-d5b-tvw) [videos](https://www.irif.fr/~letouzey/pf5/videos).

#### Cours 6 (semaine du 12/10)
  - **Type option et exceptions** : [sketch](https://sketch.sh/s/LNYzzbJLVpKgRIEYW5l2pM/), [markdown](slides/cours-06-exn.md).
  - Questions/réponses du [14/10](https://sketch.sh/s/pmgQm3hZcaHFMIEZuuNEZT/) et [15/10](https://sketch.sh/s/ntcctHLxV6FYVAXN4epFaL/) et [liens](https://bbb-front.math.univ-paris-diderot.fr/recherche/pie-rcp-d5b-tvw) [videos](https://www.irif.fr/~letouzey/pf5/videos).

#### Cours 7 (semaine du 19/10)
  - **Entrées/sorties et graphisme** : [markdown7a](slides/cours-07a-io.md), [markdown7b](slides/cours-07b-graphics.md), [pdf](slides/cours-07-io-graphics.pdf) d'origine. Auteur initial de ce cours : R. Treinen. Videos: endroit usuel.

#### Cours 8 (semaine du 2/11)
  - **Structures impératives** : [sketch](https://sketch.sh/s/odCwxaMbe7e5NxgNAboW9e/), [markdown](slides/cours-08-imperatif.md).
  - Questions/réponses : [sketch](https://sketch.sh/s/eXCDqJMy5xpRRRVoSKbRuP/) du 5/11 (resté dans l'état brut). Vidéos: endroit usuel.

#### Cours 9 (semaine du 9/11)
  - **Etude de cas** : comment réaliser un [téléchargeur de Sketch.sh](https://github.com/letouzey/sketch-downloader/blob/master/getsketch.ml) grâce à OCaml. Code à regarder, plus d'explication dans la video du jeudi 12/11.
  
#### Cours 10 (semaine du 16/11)
  - Retour sur **l'évaluation** d'un programme OCaml (à savoir sa *sémantique*) : [pdf](slides/2019/cours-06.pdf) 2019 plus quelques [remarques](slides/cours-10-eval.md) complémentaires (grosso modo vues pendant la séance du jeudi).
  - Un petit [évaluateur d'OCaml en OCaml](slides/cours-10).

#### Cours 11 (semaine du 23/11)
  - Retour sur le **typage** en OCaml : [markdown](slides/cours-11-typage.md) de cours et [exemples](slides/cours-11-typage.ml) vus en séance.

#### Cours 12 (semaine du 30/11)
  - Quelques mots sur modules et signatures : [markdown](slides/cours-12-modules.md). Ceci ne sera pas au programme de l'examen, mais peut vous servir en projet.
  - Quelques exercices pris dans les [annales d'examen](#anciens-examens) plus bas.


## Travaux pratiques (via Learn-OCaml)

A compter du 2 novembre, passage à des TP à distance.

Planning actuel:

| Groupe   | Enseignant  | Creneau         | Salle        |
|----------|-------------|-----------------|--------------|
| Groupe 1 | P. Letouzey | Mar 13:30-15:30 | ~~2031 (+2036)~~ [BBB](https://bbb-front.math.univ-paris-diderot.fr/recherche/pie-m7s-reo-mzs) |
| Groupe 2 | G. Bernardi | Mar 10:00-12:00 | ~~2003 (+2002)~~ [Discord](https://discord.gg/qd4tjJhA) |
| Groupe 3 | B. Bauer    | Lun 13:30-15:30 | ~~2032 (+2036)~~ [BBB](https://bbb-front.math.univ-paris-diderot.fr/recherche/bau-j1j-vu2-2ne) [Discord](https://discord.gg/UEQFGRA3h6)|
| Groupe 4 | A. Guatto   | Lun 14:00-16:00 | ~~2003 (+2036)~~ [Discord](https://discord.gg/5SR3TfvkbT) |
| MathInfo | V. Padovani | Mar 13:45-15:45 | ~~2003 (+2001)~~ [BBB](https://bbb-front.math.univ-paris-diderot.fr/recherche/vin-3pn-fce-cgh) |

Les premiers TP utilisent http://pf5.ddns.net , une instance de la plateforme **Learn-OCaml** : 
- A la première connexion, saisir un "nickname" qui commence par votre numéro de groupe puis votre nom de famille, par exemple 3Skywalker
- Pour les MathInfo, mettre 5 comme numéro de groupe
- Le secret est pf5
- Notez absolument votre TOKEN. C'est grâce à lui que vous pourrez retrouver votre session lors de votre prochaine connexion.

#### TP6 : Morpion

- Code fourni : [morpion.zip](tp/morpion.zip) (Version 2, évite de planter si une police de caractère manque).
- Voir [INSTALL.md](INSTALL.md) pour les conseils d'installation d'OCaml sur sa machine.

#### TP7 : Morpion partie 2

- [Sujet du tp7](tp/tp7.pdf)
- Code fourni : [morpion-partie-2.zip](tp/morpion-partie-2.zip)

## Projet

**Nouveau** : les [CONSIGNES](projet/CONSIGNES.md) précises relatives au rendu et à l'évaluation sont disponibles (cela correspond au mail envoyé le 7 decembre).

Voir le sous-repertoire [projet](projet) et en particulier le [sujet](projet/projet.pdf) et le [README.md](projet/README.md).
Attention, il peut y avoir encore des changements et correctifs, à suivre.

Quelques liens:
  - https://fr.wikipedia.org/wiki/L-Syst%C3%A8me et le début du livre http://algorithmicbotany.org/papers/#abop
  - https://www.irif.fr/~letouzey/pf5/teaser/

## Anciens examens

- [examen 2018/2019](exams/examen1819.pdf) écrit par Michele Pagani.
  Correction : [markdown](exams/exam1819.md), [sketch](https://sketch.sh/s/dgfrHHkNzdUuf3VYTRO3Vy/).

- [examen 2019/2020](exams/examen1920.pdf) par Y. Regis-Gianas. Correction : [markdown](exams/exam1920.md).

## Références et bibliographie

OCaml ailleurs sur internet ou dans des livres. Ceci n'est clairement pas exhaustif, et privilégie les ressources accessibles gratuitement.

- Le site principal sur OCaml : https://ocaml.org
- Le manuel d'OCaml : https://ocaml.org/releases/latest/manual.html
- Une bibliographie complète : https://ocaml.org/learn/books.html
- Les [notes de cours](http://www.enseignement.polytechnique.fr/profs/informatique/Jean-Christophe.Filliatre/14-15/INF549/ocaml.pdf) de J.C. Filliâtre à l'X. L'essentiel d'OCaml en 50 pages!
  Et aussi un petit [résumé de la syntaxe](http://www.enseignement.polytechnique.fr/profs/informatique/Jean-Christophe.Filliatre/14-15/INF549/memo-java-ocaml.pdf) comparée avec celle de Java.

#### Quelques Livres

- [Développement d'applications avec OCaml](https://www-apr.lip6.fr/~chaillou/Public/DA-OCAML/index.html), E. Chailloux, P. Manoury, B. Pagano. 2002 mais encore très pertinent!

- [Apprendre à programmer avec OCaml](http://programmer-avec-ocaml.lri.fr/), S. Conchon, J.C. Filliâtre, 2014. Seulement partiellement disponible en ligne, mais beaucoup d'exemples et d'exercices.

- [Real World OCaml](https://dev.realworldocaml.org/), Y. Minsky, A. Madhavapeddy, J. Hickey, 2013. Je le mentionne car ce livre est très connu, mais **attention** même si ce livre parle évidemment d'OCaml il utilise dès le début une bibliothèque alternative (`Base` et `Core`) qui diffère sensiblement de la bibliothèque standard d'OCaml.

- [Unix System Programming in OCaml](http://ocaml.github.io/ocamlunix/), X. Leroy, D. Rémy, 2010.

- [Le langage Caml](http://caml.inria.fr/pub/distrib/books/llc.pdf) P. Weis, X Leroy, 1993-1999.
  Le tout premier livre sur Caml. **Attention** ce livre traite de Caml Light, le précurseur d'OCaml, il y a donc de sensibles différences par endroit (syntaxe, bibliothèques). Mais cela reste une lecture passionnante.
