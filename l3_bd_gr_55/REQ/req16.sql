\echo req16 les produits dont le prix datant du 2021 -05 -01 est inferieur à son pique 
WITH prixMax as (
    SELECT id_produit,
        max(prix) prix
        /*table des prix max*/
    FROM Historique
    GROUP BY id_produit
)
SELECT id_produit,
    nom,
    taille
FROM (
        Produits
        NATURAL JOIN Historique
    ) as t
WHERE t.date_vendu = '2021-05-01 00:00:00'::date
    AND t.prix < (
        SELECT p.prix
        FROM prixMax p
        WHERE t.id_produit = p.id_produit
    );