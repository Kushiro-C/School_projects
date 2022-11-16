# Architecture temporaire du projet TSH

### Arbre Complet (Temporaire) du projet

Voici l'arbre complet du projet qu'on obtient après compilation, 
avec la commande `make`.


```
systl3_projet_gr_16
├── ARCHITECTURE.md
├── AUTHORS
├── Dockerfile
├── Makefile
├── README.md
├── src
│   ├── commands
│   │   ├── my_cat.c
│   │   ├── my_cp.c
│   │   ├── my_ls.c
│   │   ├── my_mkdir.c
│   │   ├── my_mv.c
│   │   ├── my_rm.c
│   │   ├── my_rmdir.c
│   │   └── my_shell.c
│   ├── exec
│   │   ├── my_cat
│   │   ├── my_cp
│   │   ├── my_ls
│   │   ├── my_mkdir
│   │   ├── my_mv
│   │   ├── my_rm
│   │   └── my_rmdir
│   ├── include
│   │   ├── string_parser.c
│   │   ├── string_parser.h
│   │   ├── string_parser.o
│   │   ├── tar.h
│   │   ├── tar_parser.c
│   │   ├── tar_parser.h
│   │   └── tar_parser.o
│   └── tests
│       └── string_parser_test.c
└── tsh
```

---

Le code des commandes et du shell tsh sont dans le répertoire `src/commands`, 
après compilation avec `make`, le répertoire `src/exec` se rempli 
d'exécutables pour chacune des commandes.

Dans `src/include` se situe les 2 fichiers `string_parser.c` et 
`tar_parser.c` qui contiennent la plupart des fonctions qu'on a 
utilisé pour manipuler des chaînes de caractères et des tarballs.


Le répertoire `src/tests` contient des tests pour nos "parsers".

Dans la racine du projet, on a le fichier exécutable `tsh`, 
qui est le lanceur de notre shell.

Il suffira d'exécuter `./tsh` dans la racine du projet pour lancer notre shell.


## Stratégie adoptée pour répondre au sujet

Pour réaliser ce projet, nous avions décidé de programmer les commandes 
du shell une à une dans des fichiers différents, afin de pouvoir tester 
nos codes manuellement, avant de les intégrer dans le shell.


On a voulu faire un shell assez simpliste, qui effectue ce qu'un shell
devrait faire, c'est-à-dire, `lire`, `évaluer`, `afficher` et `boucler` (REPL).

Pour la partie `lire`, `afficher` et `boucler`, il était assez simple d'y 
répondre, il nous suffisait de faire boucler à l'infini, et d'attendre 
l'utilisateur qui entre des commandes, pour ensuite traiter les commandes 
et afficher ce qui était attendu.

Mais pour cela, il nous fallait un `parser`, pour pouvoir "comprendre", 
"couper" et `évaluer` les commandes entrées. On a alors rapidement commencé 
à programmer des fonctions pour répondre à nos besoins. (string_parser)


**Voici ce qu'on avait en tête quant à l'architecture de nos programmes**

```
src
├── commands
│   ├── my_cat.c
│   ├── my_cp.c
│   ├── my_ls.c
│   ├── my_mkdir.c
│   ├── my_mv.c
│   ├── my_rm.c
│   ├── my_rmdir.c
│   └── my_shell.c
├── exec
│   ├── my_cat
│   ├── my_cp
│   ├── my_ls
│   ├── my_mkdir
│   ├── my_mv
│   ├── my_rm
│   └── my_rmdir
├── include
│   ├── string_parser.c
│   ├── string_parser.h
│   ├── string_parser.o
│   ├── tar.h
│   ├── tar_parser.c
│   ├── tar_parser.h
│   └── tar_parser.o
├── tests
│   ├── string_parser_test
│   └── string_parser_test.c
└── tsh
```