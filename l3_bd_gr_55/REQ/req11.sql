\echo req11 les clients et leurs produits qui ont été remboursé
SELECT nom,
    prenom,
    id_client,
    id_order
FROM (
        SELECT id_client,
            id_order
        FROM Rembours_retour
            NATURAL JOIN OrderItem
            NATURAL JOIN Commande
        WHERE statut = 'remboursé'
    ) client_order
    NATURAL JOIN Client;