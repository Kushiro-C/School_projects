██████╗░███████╗░█████╗░██████╗░███╗░░░███╗███████╗
██╔══██╗██╔════╝██╔══██╗██╔══██╗████╗░████║██╔════╝
██████╔╝█████╗░░███████║██║░░██║██╔████╔██║█████╗░░
██╔══██╗██╔══╝░░██╔══██║██║░░██║██║╚██╔╝██║██╔══╝░░
██║░░██║███████╗██║░░██║██████╔╝██║░╚═╝░██║███████╗
╚═╝░░╚═╝╚══════╝╚═╝░░╚═╝╚═════╝░╚═╝░░░░░╚═╝╚══════╝

Notre base de données est celle d'une boutique de pret à porter en ligne.
Le client se connecte pour ajouter des produits à son panier. Il peut
ensuite commander ces vetements. Il aura accès à un suivi des différents
produits achetés. Il a la possibilité de retourner et de se faire rembourser
avant x jours.

Nous avons décidé de faire 2 dossier CSV, l'un (CSV) contenant moins
de données et l'autre (CSV2) plus.

Pour lancer le projet depuis la racine :
psql
\i tables.sql
\i importcsv.sql
\i requetes.sql ou \i REQ\req1.sql

plusieurs scripts importcsv et importcsv2 sont disponibles
selon ou l'on se situe.

Patrick Chang 71802046
Vincent Du    71801856