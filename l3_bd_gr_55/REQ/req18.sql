\echo req18 tous les id_order qui ont trop été remboursé
SELECT o.id_order
FROM (
        OrderItem
        NATURAL JOIN Commande
    ) o
WHERE (
        SELECT montant
        FROM Rembours_retour r
        WHERE o.id_order = r.id_order
    ) > (
        SELECT h.prix
        FROM (
                Produit
                NATURAL JOIN Historique
                NATURAL JOIN OrderItem
            ) h
        WHERE h.date_vendu <= o.date_cmd
            AND o.id_order = h.id_order
        ORDER BY date_vendu DESC
        LIMIT 1
    );