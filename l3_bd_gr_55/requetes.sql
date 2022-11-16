\echo req1 nombre moyen de produit par commande:
SELECT AVG(count)
FROM (SELECT count(id_cmd)
 FROM OrderItem GROUP BY id_cmd)
as count;

\echo req2 id des clients ayant commandé apres 2021 la marque Gacci
SELECT DISTINCT id_client FROM OrderItem NATURAL JOIN Commande
WHERE id_produit 
IN (SELECT id_produit FROM Produits WHERE marque = 'Gacci')
AND date_cmd >= '2021-01-01 00:00:00'::date;

\echo req3 total des paniers de chaque client des produits qui existent en stock à la quantité voulue
SELECT Client.id_client, COALESCE(SUM(prix * quantite), 0)
FROM Client LEFT JOIN (Panier NATURAL JOIN Produits)
ON Client.id_client = Panier.id_client
WHERE (stock != 0 OR stock IS NULL) AND stock >= quantite
GROUP BY Client.id_client;

\echo req4 meilleurs clients souhaitant acheter des produits de chaque marque en quantité
WITH ClientMarque(id_client, marque, q) AS /*table exprimant la quantité de produit par marque de chaque client*/
(SELECT id_client, marque, SUM(quantite)
FROM Produits NATURAL JOIN Panier GROUP BY id_client, marque),
MaxTop(marque, qte) AS /*max des quantité par marque*/
(SELECT marque, MAX(q) as qte FROM ClientMarque GROUP BY marque)
SELECT marque, id_client FROM ClientMarque NATURAL JOIN MaxTop
WHERE q = qte;

\echo req5 Pour chaque matiere nombre de produit composé de celui ci
SELECT nom_matiere, count(*) FROM
(SELECT DISTINCT id_produit, id_matiere
FROM Composition) as distin NATURAL JOIN Matiere
GROUP BY nom_matiere;

\echo req6 id des produits qui sont en préparation pour une commande
SELECT id_produit FROM Produits WHERE id_produit IN
(SELECT id_produit FROM (OrderItem NATURAL JOIN Commande) WHERE etat = 'preparation');

\echo req7 les produits les plus recherchés actuellement au dessus de la moyenne
SELECT nom, id_produit, taille, SUM(quantite)
FROM Produits NATURAL JOIN Panier
GROUP BY nom, id_produit, taille
HAVING SUM(quantite) > AVG(quantite);

\echo req8 les clients qui ne pourront pas acheter des produits faute de stock
SELECT DISTINCT nom, prenom, id_client
FROM Client c NATURAL JOIN Panier p
WHERE NOT EXISTS (SELECT * FROM Produits
WHERE stock >= p.quantite AND id_produit = p.id_produit);


\echo req9 delai de livraison pour la commande numéro 7
SELECT id_produit, nom, (date_livraison - date_expedition) as temps
FROM OrderItem NATURAL JOIN SuiviProduit NATURAL JOIN Produits
WHERE id_cmd = 7;

\echo req10 les produits le plus present dans chaque panier
SELECT c.nom, prenom, id_produit ,MAX(quantite) nbProduit
FROM Client c NATURAL JOIN Panier
GROUP BY c.nom, prenom, id_produit,quantite;

\echo req11 les clients et leurs produits qui ont été remboursé
SELECT nom, prenom, id_client, id_order
FROM (SELECT id_client, id_order 
FROM Rembours_retour NATURAL JOIN OrderItem NATURAL JOIN Commande
WHERE statut = 'remboursé') client_order
NATURAL JOIN Client;

\echo req12 :
\echo nombre de produit dans la commande numero 3
\echo verifiez en externe si > 2 alors trouver le montant (rembourser?) du produit le moins cher
\echo req12a
SELECT COUNT(*) FROM Commande NATURAL JOIN OrderItem WHERE id_cmd = 3;
\echo  req12b trouver le prix du produit le moins cher de la commande numéro 3
SELECT MIN(prix) FROM Commande NATURAL JOIN OrderItem NATURAL JOIN Produits
WHERE id_cmd = 3;
\echo res12c le produit le moins cher de la commande 3
SELECT id_order, nom, prix FROM Commande NATURAL JOIN OrderItem NATURAL JOIN Produits
WHERE id_cmd = 3 ORDER BY prix ASC LIMIT 1;

\echo req13 produit exclusivement fabriqué en lin et disponible en stock
SELECT id_produit FROM (Produits p NATURAL JOIN Composition NATURAL JOIN Matiere) info
WHERE  nom_matiere = 'lin' AND stock > 0 AND id_produit
IN (SELECT id_produit FROM Composition NATURAL JOIN Matiere
GROUP BY id_produit HAVING count(id_produit) = 1); /*produit fabriqué avec que 1 matiere*/

\echo req14 tous les produits étant composé de toutes les matieres avec agregation
SELECT id_produit FROM Composition
GROUP BY id_produit HAVING count(DISTINCT id_matiere)
= (select count(DISTINCT id_matiere) FROM Matiere);

\echo req15 tous les produits étant composé de toutes les matieres avec sous req correlées
SELECT DISTINCT c1.id_produit FROM Composition c1
WHERE NOT EXISTS (SELECT m.id_matiere FROM Matiere m
WHERE NOT EXISTS (SELECT c2.id_produit FROM Composition c2
WHERE m.id_matiere = c2.id_matiere AND
c1.id_produit = c2.id_produit));

\echo req16 les produits dont le prix datant du 2021-05-01 est inferieur à son pique
WITH prixMax as (SELECT id_produit, max(prix) prix /*table des prix max*/
FROM Historique GROUP BY id_produit)
SELECT id_produit, nom, taille
FROM (Produits NATURAL JOIN Historique) as t 
WHERE t.date_vendu = '2021-05-01 00:00:00'::date
AND t.prix < 
(SELECT p.prix FROM prixMax p WHERE t.id_produit = p.id_produit);

\echo req17 Pour si montant du remboursement est correct pour un client spécifique
\echo donnez le montant que le client perd pour le produit d orderItem numero 8
\echo si le montant est positif nous avons plus remboursé le client qu il a payé
WITH infoOrder AS /*information sur l'id order 8*/
(SELECT * FROM (OrderItem NATURAL JOIN Rembours_retour NATURAL JOIN Commande)
 o WHERE o.id_order = 8)
SELECT (
    (SELECT montant FROM infoOrder) /*montant remoboursé*/
    - /*- prix lors de l'achat*/
    (SELECT h.prix FROM (Produit NATURAL JOIN Historique NATURAL JOIN OrderItem) h
    WHERE h.date_vendu <= 
    (SELECT date_cmd FROM infoOrder) 
    AND id_produit = (SELECT id_produit FROM infoOrder)
    ORDER BY date_vendu DESC LIMIT 1)
) AS perdu;

\echo req18 tous les id_order qui ont trop été remboursé
SELECT o.id_order FROM (OrderItem NATURAL JOIN Commande)o
WHERE (SELECT montant FROM Rembours_retour r WHERE o.id_order = r.id_order)
> (SELECT h.prix FROM (Produit NATURAL JOIN Historique NATURAL JOIN OrderItem) h
    WHERE h.date_vendu <= o.date_cmd AND o.id_order = h.id_order
    ORDER BY date_vendu DESC LIMIT 1);

\echo req19a les pires avis de produits
SELECT a1.id_client, a1.id_produit, a1.note FROM Avis a1
WHERE NOT EXISTS (SELECT * FROM Avis a2
WHERE a2.note < a1.note);

\echo req20a pires avis 2
SELECT id_client, id_produit, note FROM Avis
WHERE note <= ALL(SELECT note FROM Avis);

\echo req19b pires avis ignorant NULL
SELECT a1.id_client, a1.id_produit, a1.note FROM Avis a1
WHERE NOT EXISTS (SELECT * FROM Avis a2
WHERE a2.note < a1.note) AND note IS NOT NULL;

\echo req20b pires avis 2 ignorant NULL
SELECT id_client, id_produit, note FROM Avis
WHERE note <= ALL(SELECT note FROM Avis WHERE note IS NOT NULL);

\echo bonus nom des produits ayant un avis (natural join) :
SELECT DISTINCT nom FROM Avis NATURAL JOIN Produits;

/*SELECT id_produit FROM Composition NATURAL JOIN Matiere GROUP BY id_produit HAVING count(id_produit) = 1;

SELECT id_produit ,COUNT(id_produit) FROM Composition NATURAL JOIN Matiere GROUP BY id_produit*/


/*\echo nom des produits le plus present dans chaque panier 
WITH client_qt(nom, prenom, id_produit,quantite) AS (SELECT c.nom, prenom, id_produit ,MAX(quantite) nbProduit
FROM Client c NATURAL JOIN Panier
GROUP BY c.nom, prenom, id_produit,quantite)
SELECT * FROM client_qt NATURAL JOIN Produits p;*/

/* test moyenne note null
SELECT id_produit, AVG(note) FROM Avis NATURAL JOIN Produits
GROUP BY id_produit;

SELECT id_produit, AVG(note) FROM Avis NATURAL LEFT JOIN Produits
GROUP BY id_produit;*/

/*-----test condition totalité-----*/
\echo
/*SELECT DISTINCT pr1.nom
FROM Produits pr1
WHERE NOT EXISTS (SELECT pa.id_panier
 FROM Panier pa
 WHERE NOT EXISTS (SELECT pr2.nom
 FROM Produits pr2
 WHERE pr2.id_produit = pa.id_produit
 AND
 pr1.nom = pr2.nom));*/
/*SELECT m.id_matiere
FROM Matiere m
WHERE NOT EXISTS (SELECT *
FROM Composition c WHERE NOT EXISTS (SELECT m2.id_matiere
FROM Matiere m2 WHERE c.id_matiere = m2.id_matiere AND
m2.id_matiere = m.id_matiere))*/

/*add 4,2 a composition*/
/*SELECT pr1.id_produit
FROM Produits pr1
WHERE NOT EXISTS (SELECT c.id_matiere
 FROM Composition c
 WHERE NOT EXISTS (SELECT pr2.id_produit
 FROM Produits pr2
 WHERE pr2.id_produit = c.id_produit
 AND
 pr1.id_produit = pr2.id_produit));*/
/*-------------*/


/* --------------test prix -------------
\echo produit ayant un prix inferieur au maximum qu ils ont eu
SELECT id_produit, nom, taille
FROM (Produits NATURAL JOIN Historique) as t 
WHERE prix < (SELECT max(prix) FROM Historique WHERE id_produit = t.id_produit)
GROUP BY id_produit;

SELECT id_produit, max(prix) FROM Historique GROUP BY id_produit;

WITH prixMax as (SELECT id_produit, max(prix) FROM Historique GROUP BY id_produit)
SELECT id_produit, nom, taille
FROM (Produits NATURAL JOIN Historique) as t 
WHERE t.date_vendu = '21-05-01' AND t.prix <= prixMax.prix AND t.id_produit = prixMax.id_produit;

\echo les produits dont le prix actuel est inferieur à son pique
WITH prixMax as (SELECT id_produit, max(prix) prix FROM Historique GROUP BY id_produit)
SELECT id_produit, nom, taille
FROM (Produits NATURAL JOIN Historique) as t 
WHERE t.date_vendu = '2021-05-01 00:00:00'::date AND t.prix < (SELECT p.prix FROM prixMax p WHERE t.id_produit = p.id_produit);
-----------------------------*/