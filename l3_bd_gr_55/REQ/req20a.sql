\echo req20a pires avis 2
SELECT id_client,
    id_produit,
    note
FROM Avis
WHERE note <= ALL(
        SELECT note
        FROM Avis
    );