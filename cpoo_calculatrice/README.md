# Projet : Calculatrice évoluée

---



## Pour exécuter le projet :

`./gradlew build`

- Ensuite on lance notre application :

`java -jar build/libs/cpoo_calculatrice.jar`

- Ou on peut lancer les tests :

`./gradlew test`

- Pour nettoyer le projet :

`./gradlew clean`


---


## Concernant l'organisation du projet :

Nous avons décidé d'utiliser Gradle afin d'organiser le projet. Les tests sont donc situés dans `src/test/java/calc/`
et les classes dans  `src/main/java/calc`.

`App.java` est la classe "Launcher", qui lance le programme, c'est elle qui contient le `main`. 

`Shell.java` est la classe contenant la partie permettant de lire ce que l'utilisateur écrit.

`MyParser.java` permet de comprendre ce que tape l'utilisateur. Il permet de vérifier si celui-ci entre une opération ou bien une variable.

`Calculator.java` contient la partie concernant le calcul des entiers (Integer) et des décimaux (BigDecimal).

Et `BooleanCalculator.java` calcul les opérations utilisant les booleans.

Une image `diagram.jpg`, ainsi qu'une `javadoc`(dans `javadoc/calc/` pour chacune des classes) est disponible pour mieux comprendre l'organisation.


---
#### Lien du git :

https://gaufre.informatique.univ-paris-diderot.fr/du/cpoo_calculatrice