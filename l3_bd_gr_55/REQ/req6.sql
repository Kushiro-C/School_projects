\echo req6 id des produits qui sont en préparation pour une commande
SELECT id_produit
FROM Produits
WHERE id_produit IN (
    SELECT id_produit
    FROM (
            OrderItem
            NATURAL JOIN Commande
        )
    WHERE etat = 'preparation'
);