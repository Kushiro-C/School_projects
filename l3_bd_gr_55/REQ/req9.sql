\echo req9 delai de livraison pour la commande numéro 7
SELECT id_produit,
    nom,
    (date_livraison - date_expedition) as temps
FROM OrderItem
    NATURAL JOIN SuiviProduit
    NATURAL JOIN Produits
WHERE id_cmd = 7;