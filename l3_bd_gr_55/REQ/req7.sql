\echo req7 les produits les plus recherchés actuellement au dessus de la moyenne
SELECT nom,
    id_produit,
    taille,
    SUM(quantite)
FROM Produits
    NATURAL JOIN Panier
GROUP BY nom,
    id_produit,
    taille
HAVING SUM(quantite) > AVG(quantite);