\echo req3 total des paniers de chaque client des produits qui existent en stock à la quantité voulu
SELECT Client.id_client,
    COALESCE(SUM(prix * quantite), 0)
FROM Client
    LEFT JOIN (
        Panier
        NATURAL JOIN Produits
    ) ON Client.id_client = Panier.id_client
WHERE (
        stock != 0
        OR stock IS NULL
    )
    AND stock >= quantite
GROUP BY Client.id_client;