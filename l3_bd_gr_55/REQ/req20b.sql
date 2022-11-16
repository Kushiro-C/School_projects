\echo req20b pires avis 2 ignorant null
SELECT id_client,
    id_produit,
    note
FROM Avis
WHERE note <= ALL(
        SELECT note
        FROM Avis
        WHERE note IS NOT NULL
    );