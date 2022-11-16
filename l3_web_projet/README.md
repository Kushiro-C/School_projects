# Projet Programmation Web

---

### Membres du groupe :

Patrick Chang 71802046
Vincent Du    71801856

---

### Configurations :

- Afin de configurer la base de données MySQL, veuillez modifiez le fichier `config.js`
en remplaçant les données indiquées si nécessaire.

- Ensuite exécutez le fichier en entrant la commande `./init_db` dans `/l3_web_projet`


### Compilation et Exécution du programme :

- Toujours dans `/l3_web_projet`, lancez la commande `npm install` puis `npm start`

- Ensuite sur votre navigateur préféré, allez sur `localhost:8080/`

- Pour vous connecter sur notre site, on a mis à disposition 6 utilisateurs,
les emails étant `a@b` avec `n` fois `a`, et `n` allant de 1 à 6,
et le mot de passe commun à tous est `mdp1`.
*On peut donc se connecter avec les identifiants `aaaaa@b` et `mdp1` par exemple*


### Répartition du travail :

- Vincent s'est d'abord occupé de poser les bases, avec la page `home.ejs`
qu'on affiche, ainsi que le serveur dans `main.js` utilisant `express`.
Les publications étaient affichées, mais avec actualisation de pages.
Il s'est également occupé de gérer une grande partie du côté visuel avec `bootstrap` et du `CSS`

- Patrick a ensuite pris la relève, et a rajouté un peu de `CSS`,
puis réorganisé ce qui était nécessaire afin de rendre la page actualisée
automatiquement, sans avoir à rafraîchir la page, en utilisant `AJAX`.
On avait ainsi les publications et le serveur reliés automatiquement.
