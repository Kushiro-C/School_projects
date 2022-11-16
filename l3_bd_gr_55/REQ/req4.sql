\echo req4  id des meilleurs clients souhaitant acheter des produits de chaque marque en quantité 
WITH ClientMarque(id_client, marque, q) AS
/*table exprimant la quantité de produit par marque de chaque client*/
(
    SELECT id_client,
        marque,
        SUM(quantite)
    FROM Produits
        NATURAL JOIN Panier
    GROUP BY id_client,
        marque
),
MaxTop(marque, qte) AS
/*max des quantité par marque*/
(
    SELECT marque,
        MAX(q) as qte
    FROM ClientMarque
    GROUP BY marque
)
SELECT marque,
    id_client
FROM ClientMarque
    NATURAL JOIN MaxTop
WHERE q = qte;