\echo req12 :
\echo nombre de produit dans la commande numero 3 \ echo verifiez en externe si > 2 
\echo alors trouver le montant (rembourser ?) du produit le moins cher 
\echo req12a
SELECT COUNT(*)
FROM Commande
    NATURAL JOIN OrderItem
WHERE id_cmd = 3;
\echo req12b trouver le prix du produit le moins cher de la commande numéro 3
SELECT MIN(prix)
FROM Commande
    NATURAL JOIN OrderItem
    NATURAL JOIN Produits
WHERE id_cmd = 3;
\echo res12c le produit le moins cher de la commande 3
SELECT id_order,
    nom,
    prix
FROM Commande
    NATURAL JOIN OrderItem
    NATURAL JOIN Produits
WHERE id_cmd = 3
ORDER BY prix ASC
LIMIT 1;