\echo req14 tous les produits étant composé de toutes les matieres avec agregation
SELECT id_produit
FROM Composition
GROUP BY id_produit
HAVING count(DISTINCT id_matiere) = (
        select count(DISTINCT id_matiere)
        FROM Matiere
    );