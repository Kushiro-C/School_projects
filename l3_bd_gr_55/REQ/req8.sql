\echo req8 les clients qui ne pourront pas acheter des produits faute de stock
SELECT DISTINCT nom,
    prenom,
    id_client
FROM Client c
    NATURAL JOIN Panier p
WHERE NOT EXISTS (
    SELECT *
    FROM Produits
    WHERE stock >= p.quantite
        AND id_produit = p.id_produit
);