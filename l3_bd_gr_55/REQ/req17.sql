\echo req17 Pour si montant du remboursement est correct pour un client spécifique 
\echo donnez le montant que le client perd pour le produit d orderItem numero 8 
\echo si le montant est positif nous avons plus remboursé le client qu il a payé 
WITH infoOrder AS
/*information sur l'id order 8*/
(
    SELECT *
    FROM (
            OrderItem
            NATURAL JOIN Rembours_retour
            NATURAL JOIN Commande
        ) o
    WHERE o.id_order = 8
)
SELECT (
        (
            SELECT montant
            FROM infoOrder
        )
        /*montant remoboursé*/
        -
        /*- prix lors de l'achat*/
        (
            SELECT h.prix
            FROM (
                    Produit
                    NATURAL JOIN Historique
                    NATURAL JOIN OrderItem
                ) h
            WHERE h.date_vendu <= (
                    SELECT date_cmd
                    FROM infoOrder
                )
                AND id_produit = (
                    SELECT id_produit
                    FROM infoOrder
                )
            ORDER BY date_vendu DESC
            LIMIT 1
        )
    ) AS perdu;