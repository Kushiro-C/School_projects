\echo req10 les produits le plus present dans chaque panier
SELECT c.nom,
    prenom,
    id_produit,
    MAX(quantite) nbProduit
FROM Client c
    NATURAL JOIN Panier
GROUP BY c.nom,
    prenom,
    id_produit,
    quantite;