\echo req19a les pires avis de produits
SELECT a1.id_client,
    a1.id_produit,
    a1.note
FROM Avis a1
WHERE NOT EXISTS (
        SELECT *
        FROM Avis a2
        WHERE a2.note < a1.note
    );