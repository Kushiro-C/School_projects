# Projet : Base de Données Avancées

Patrick Chang 71802046

Vincent Du    71801856

Groupe TP Mercredi

---

## Modélisation

La modélisation de pré-soutenance se trouve dans `MODELISATION/PRE-SOUTENANCE/modelisation.pdf`

La nouvelle modélisation avec en rouge les tables non implémentées (non utilisées mais elles existent) se trouve dans `MODELISATION/FINAL/modelisation_v2.pdf`

---

## Tout préparer pour les tests

On peut lancer `psql` depuis le répertoire racine `bd_chang_du_m1/`, et exécuter

`\i CREATION/init1.sql`

ou (recommandé) pour tester :

`\i CREATION/init2.sql`

Cela prépare l'ensemble de données nécessaires pour le CSV2 pour tester le projet.

#### Les 2 parties suivantes ne sont pas nécessaires si la commande du dessus a été exécutée. (voir directement section TESTS)

---

## Créer et Peupler les tables

Depuis le répertoire racine `bd_chang_du_m1/`, lancez `psql`.

Pour créer les tables, entrez la commande :

`\i CREATION/tables.sql`

Vous êtes automatiquement mis sur la base de données `bd_chang_du_m1`.

Ensuite on peuple les tables soit avec le contenu de `CREATION/CSV1/` qui est plus dense, soit avec le celui de `CREATION/CSV2/` qui est plus léger, et plus pratique pour tester :

`\i CREATION/importCSV1.sql`

`\i CREATION/importCSV2.sql`

---

## Triggers et Fonctions

Pour créer nos triggers, on lance la commande :

`\i CREATION/create_triggers.sql`

Les autre fonctions qui ne sont pas utilisées pour définir les triggers se trouvent dans le fichier :

`create_functions.sql`

---

## Tests

Les tests se trouvent dans le répertoire `TESTS/` à la racine du projet.

Naviguez-y vers ce répertoire, et lancez les tests que vous voulez dans `psql` avec la commande :

`\i nom_du_fichier_test.sql`

