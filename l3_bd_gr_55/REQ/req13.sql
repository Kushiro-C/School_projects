\echo req13 produit exclusivement fabriqué en lin et disponible en stock
SELECT id_produit
FROM (
        Produits p
        NATURAL JOIN Composition
        NATURAL JOIN Matiere
    ) info
WHERE nom_matiere = 'lin'
    AND stock > 0
    AND id_produit IN (
        SELECT id_produit
        FROM Composition
            NATURAL JOIN Matiere
        GROUP BY id_produit
        HAVING count(id_produit) = 1
    );
/*produit fabriqué avec que 1 matiere*/