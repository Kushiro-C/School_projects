\echo req5 Pour chaque matiere nombre de produit composé de celui ci
SELECT nom_matiere,
    count(*)
FROM (
        SELECT DISTINCT id_produit,
            id_matiere
        FROM Composition
    ) as distin
    NATURAL JOIN Matiere
GROUP BY nom_matiere;