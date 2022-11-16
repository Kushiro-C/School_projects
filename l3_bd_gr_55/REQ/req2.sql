\echo req2 id des clients ayant commandé apres 2021 la marque Gacci
SELECT DISTINCT id_client
FROM OrderItem
    NATURAL JOIN Commande
WHERE id_produit IN (
        SELECT id_produit
        FROM Produits
        WHERE marque = 'Gacci'
    )
    AND date_cmd >= '2021-01-01 00:00:00'::date;