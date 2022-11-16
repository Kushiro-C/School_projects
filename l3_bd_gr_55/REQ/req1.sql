\echo req1 nombre moyen de produit par commande:
SELECT AVG(count)
FROM (
        SELECT count(id_cmd)
        FROM OrderItem
        GROUP BY id_cmd
    ) as count;